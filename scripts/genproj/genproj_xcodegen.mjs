// @ts-check

// @ts-ignore: node modules
import fs from 'fs';

// @ts-ignore: node modules
import path from 'path/posix';

import YAML from '../js/yaml.mjs';

import {genprojProcessSources} from './genproj_process_sources.mjs';
import {getFileExt, getFilesFromPath} from './genproj_tools.mjs';
import {loadConfigParams, processValueBody} from './genproj_config.mjs';

export function generateXcodeGen(genParams, configParams) {
    loadConfigParams(genParams.projectConfig, configParams);

    //console.log(configParams);

    function processVar(value) {
        return processValueBody(`\${CONFIG_${value}}`, configParams, true);
    }

    function makeArrayValue(object, prop, name, first, deflt) {
        let array = [];
        pushArrayValue(array, name, first);
        if (array.length) 
            object[prop] = array;
        else
            if (deflt) {
                object[prop] = deflt;
                if (first !== undefined) object[prop].unshift(first);
            }
    }

    function pushArrayValue(array, name, first) {
        let res = processVar(name);
        if (res) {
            if (first !== undefined) array.push(first);

            if (Array.isArray(res)) for (let r of res) array.push(r);
            else array.push(res);
        }
    }

    let platformName = processVar('PROJECT_PLATFORM') ?? 'iOS';

    let platformVars = new Map();
    platformVars.set('AND', '&&');
    platformVars.set('OR', '||');
    platformVars.set('NOT', '!');
    platformVars.set('APPLE', true);
    platformVars.set('UNIX', true);
    platformVars.set('IOS', platformName.toUpperCase() === 'IOS');
    platformVars.set('MACOS', platformName.toUpperCase() === 'MACOS');

    if (genParams.vars) {
        for (let varIt of genParams.vars) {
            let varParams = varIt.split('=');
            let varName = varParams[0];
            let varValue = varParams.length > 1 ? varParams[1] : true;
            platformVars.set(varName, varValue);
        }
    }

    function evalIf(group) {
        if (group.if === undefined) return true;

        let expr = group.if.replaceAll('(', ' ( ');
        expr = expr.replaceAll(')', ' ) ');

        let exprArr = expr.split(' ');
        let repArr = [];

        for (let el of exprArr) {
            let tel = el.trim();
            if (tel.trim() === '') continue;

            let uel = tel.toUpperCase();
            if (tel[0].toLowerCase() === uel[0]) {
                repArr.push(tel);
                continue;
            }

            if (platformVars.has(uel)) {
                repArr.push(platformVars.get(uel));
                continue;
            }

            if (configParams.has(uel)) {
                repArr.push(configParams.get(uel));
                continue;
            }

            repArr.push('false');
        }

        expr = repArr.join(' ');

        //console.log(expr);

        return eval(expr);
    }

    function createCustomGroup(group) {

        group.sourceDir = '';
        group.sourceFiles = new Set();

        if (group.files) {
            if (Array.isArray(group.files)) {
                group.sourceDir = path.dirname(group.files[0]);
                for (let file of group.files)
                    group.sourceFiles.add(path.basename(file));
            } else {
                group.sourceDir = path.dirname(group.files);
                group.sourceFiles.add(path.basename(group.files));
            }
        } else {
            if (Array.isArray(group.mask)) {
                group.sourceDir = path.dirname(group.mask[0]);
                for (let i = 0 ; i < group.mask.length; ++i) {
                    group.mask[i] = path.basename(group.mask[i]);
                }
            } else {
                group.sourceDir = path.dirname(group.mask);
                group.mask = path.basename(group.mask);
            }

            let files = getFilesFromPath(group.sourceDir, group);

            for (let file of files) {
                group.sourceFiles.add(file);
            }
        }
        return group;
    }

    let xcodeProjFile = genParams.targetXcodeGen;

    if (genParams.targetDir) {
        fs.mkdirSync(genParams.targetDir, {recursive: true});

        xcodeProjFile = path.join(genParams.targetDir, genParams.targetXcodeGen);
    }

    //
    // Common data
    //
    let includeDirectories = ['$(inherited)'];

    pushArrayValue(includeDirectories, 'IOS_INCLUDE_DIRECTORIES');
    pushArrayValue(includeDirectories, 'INCLUDE_DIRECTORIES');
    pushArrayValue(includeDirectories, 'PROJECT_INCLUDE_DIRECTORIES');

    //
    // Load source groups
    //
    let groups = [];
    let param = undefined;

    let defines = ['$(inherited)', 'HAVE_PTHREAD=1'];

    // SQlite warnings
    defines.push("SQLITE_ENABLE_LOCKING_STYLE=1")
    defines.push("HAVE_GETHOSTUUID=0")

    if ((param = processVar('PROJECT_COMPILE_DEFINITIONS'))) {
        for (let d of param) {
            if (!defines.includes(d)) defines.push(d);
        }
    }

    if ((param = processVar('PROJECT_MULTIPLAYER'))) {
        defines.push("CONFIG_PROJECT_MULTIPLAYER=1");
    }


    if ((param = processVar('PROJECT_MANGL_CONFIG'))) {
        groups.push({
            include_directories: [path.dirname(param)],
            sourceDir: path.dirname(param),
            sourceFiles: [path.basename(param)],
            sourceGroupPrefix: 'Source Files',
            sourceGroupName: 'Config',
        });
    }

    if ((param = processVar('add_resources'))) {
        let group = {};
        group._dir = path.dirname(param[0]);
        group._files = param;
        group._resource_group = 'Project';
    }

    if ((param = processVar('PROJECT_APP_ICON_ASSETS'))) {
        groups.push(createCustomGroup({
            files: param, 
            sourceGroupPrefix: 'Resources', 
            sourceGroupName: 'AppIcon',
        }));
    }

    if ((param = processVar('PROJECT_APP_LAUNCH_STORYBOARD'))) {
        groups.push(createCustomGroup({
            files: param, 
            sourceGroupPrefix: 'Resources', 
            sourceGroupName: 'Launch/Storyboard',
        }));
    }

    if ((param = processVar('PROJECT_APP_LAUNCH_ASSETS'))) {
        groups.push(createCustomGroup({
            mask: param, 
            sourceGroupPrefix: 'Resources', 
            sourceGroupName: 'Launch/Assets',
        }));
    }

    if ((param = processVar('PROJECT_RESOURCE_FILES'))) {
        groups.push(createCustomGroup({
            files: param, 
            sourceGroupPrefix: 'Resources', 
            sourceGroupName: 'Additional project resources',
        }));
    }

    if ((param = processVar('PROJECT_RESOURCES'))) {
        groups.push(createCustomGroup({
            files: param, 
            sourceGroupPrefix: 'Resources', 
            sourceGroupName: 'Resources',
        }));
    }


    if (Array.isArray(genParams.sources)) {
        for (let yaml of genParams.sources) {
            const doc = YAML.parse(fs.readFileSync(yaml, 'utf8'));
            let sourcesData = genprojProcessSources(doc.sources, configParams);
            groups.push.apply(groups, sourcesData.groups);
        }
    } else {
        const doc = YAML.parse(fs.readFileSync(genParams.sources, 'utf8'));
        let sourcesData = genprojProcessSources(doc.sources, configParams);
        groups.push.apply(groups, sourcesData.groups);
    }

    for (let group of groups) {
        if (!evalIf(group)) continue;

        if (group.include_directories) {
            for (let dir of group.include_directories) {
                let pdir = processValueBody(dir, configParams);
                if (!includeDirectories.includes(pdir)) includeDirectories.push(pdir);
            }
        }

        if (group.compile_definitions) {
            for (let def of group.compile_definitions) {
                if (!defines.includes(def)) defines.push(def);
            }
        }
    }

    //
    // Do some Pre-processingprocessing. Add additional groups if necessary
    //

    let projectName = processVar('PROJECT_NAME');
    let projectDisplayName = processVar('PROJECT_DISPLAY_NAME');
    if (!projectDisplayName)
        projectDisplayName = projectName;


    let project = {};

    //
    // name:
    //
    project.name = projectName;

    //
    // options:
    //
    let options = {
        deploymentTarget: {},
        defaultConfig: 'Debug',
        groupSortPosition: 'top',
        groupOrdering: ['Sources', 'Resources', 'Bundle'],
    };

    // if(!platformName){
    //     platformName = "iOS"
    //     //TODO
    //     //platformTag = platformName.toUpperCase();
    // }

    options.deploymentTarget[platformName] = processVar('PROJECT_DEPLOYMENT_TARGET');

    // project.options.defaultConfig = "Debug";
    // project.options.groupSortPosition = "top";
    // project.options.groupOrdering = ["Sources", "Resources", "Bundle"];

    if (processVar('PROJECT_PODFILE')) {
        options.postGenCommand = 'pod install --repo-update';

        let podPath = processVar('PROJECT_PODFILE');
        let podFile = path.basename(podPath);

        let podLinkTarget = path.join(genParams.targetDir, podFile);

        //console.debug(podFile, podSyncTarget);
        fs.rmSync(podLinkTarget, {force: true});
        fs.symlinkSync(podPath, podLinkTarget, 'file');

    }

    project.options = options;

    //
    // configs:
    //
    let configs = {
        Debug: 'debug',
        AdHocDebug: 'debug',
        Release: 'release',
        AdHoc: 'release',
        AppStore: 'release',
    };
    project.configs = configs;

    //
    // schemes:
    // xcode schemes setup. Set archiving to appstore by default
    //
    let schemes = {};

    let debugScheme = {
        build: {
            targets: {},
            config: 'Debug',
        },
        run: {
            targets: {},
            config: 'Debug',
            environmentVariables: {
                IDEPreferLogStreaming: "YES",
            },
        },
        archive: {
            targets: {},
            config: 'AppStore',
        },
    };

    debugScheme.build.targets[projectName] = 'all';
    debugScheme.run.targets[projectName] = 'all';
    debugScheme.archive.targets[projectName] = 'all';

    schemes[projectName] = debugScheme;

    let optimizedScheme = {
        build: {
            targets: {},
            config: 'Release',
        },
        run: {
            targets: {},
            config: 'Release',
        },
    };

    optimizedScheme.build.targets[projectName] = ['build', 'run'];
    optimizedScheme.run.targets[projectName] = 'all';

    schemes.Optimized = optimizedScheme;

    project.schemes = schemes;

    //
    // settings:
    //
    let settings = {
        VALIDATE_WORKSPACE: 'NO',
        CLANG_WARN_QUOTED_INCLUDE_IN_FRAMEWORK_HEADER: 'NO',
        MARKETING_VERSION: processVar('PROJECT_VERSION'),
        CURRENT_PROJECT_VERSION: processVar('PROJECT_BUILD'),

        PRODUCT_BUNDLE_IDENTIFIER: processVar('XCODE_PRODUCT_BUNDLE_IDENTIFIER'),
        
        INFOPLIST_FILE: processVar('XCODE_INFOPLIST_FILE'),
        INFOPLIST_KEY_CFBundleDisplayName: projectDisplayName,


        DEVELOPMENT_TEAM: processVar('XCODE_DEVELOPMENT_TEAM'),
        CODE_SIGN_ENTITLEMENTS: processVar('XCODE_CODE_SIGN_ENTITLEMENTS'),

        ASSETCATALOG_COMPILER_APPICON_NAME: 'AppIcon',


        ENABLE_BITCODE: processVar('PROJECT_ENABLE_BITCODE') ?? 'NO',
        CLANG_CXX_LANGUAGE_STANDARD: processVar('XCODE_CLANG_CXX_LANGUAGE_STANDARD') ?? 'c++23',

        CLANG_WARN_COMMA: 'NO',
        CLANG_WARN_DOCUMENTATION_COMMENTS: 'NO',

        GCC_WARN_UNUSED_FUNCTION: 'NO',
        GCC_WARN_UNUSED_VARIABLE: 'NO',
        GCC_WARN_64_TO_32_BIT_CONVERSION: 'NO',

        USE_HEADERMAP: 'NO',
    };

    if ((param = processVar('PROJECT_APP_CATEGORY')))
        settings["INFOPLIST_KEY_LSApplicationCategoryType"] = param;

    // if ((param = processVar('PROJECT_TARGETED_DEVICE_FAMILY'))) {
    //     settings['TARGETED_DEVICE_FAMILY'] = param;
    // }


    //TODO:
    // outOptVal("ARCHS")
    // outOptVal("VALID_ARCHS")

    // # TODO: make it per target. Remove for release
    // if param := configParams.lval("VALIDATE_WORKSPACE_SKIPPED_SDK_FRAMEWORKS"):
    //     outParam("VALIDATE_WORKSPACE_SKIPPED_SDK_FRAMEWORKS", param)

    makeArrayValue(settings, 'OTHER_CPLUSPLUSFLAGS', 'PROJECT_OTHER_CXXFLAGS', '$(inherited)', [
        "-fcxx-modules",
        "-fmodules",
        "-fexperimental-library",
        "-fexceptions",
    ]);

    makeArrayValue(settings, 'OTHER_CFLAGS', 'PROJECT_OTHER_CFLAGS', '$(inherited)');

    makeArrayValue(settings, 'WARNING_CFLAGS', 'PROJECT_WARNING_CFLAGS', '$(inherited)', [
        '-Wno-undef', 
        '-Wno-ambiguous-macro', 
        '-Wno-deprecated-declarations',
        "-Wmissing-field-initializers",
        //"-Wno-missing-designated-field-initializers",
    ]);

    makeArrayValue(settings, 'OTHER_LDFLAGS', 'PROJECT_OTHER_LDFLAGS', '$(inherited)', [
        '-all_load', 
        '-ObjC',
    ]);

    // Include directories
    settings.HEADER_SEARCH_PATHS = includeDirectories;

    // Now include project level includes. Must come last so by accident project headers won't be picked up instead of systemwide
    //TODO: read sources from yaml?
    // for (const group of sourceGroups){
    //     if(evalCondition(group)){
    //         outListExpanded(group.includeDirectories)
    //     }
    // }
    // outParamEnd(true)

    //
    // Framework search paths
    //
    makeArrayValue(settings, 'FRAMEWORK_SEARCH_PATHS', 'PROJECT_FRAMEWORK_SEARCH_PATHS', '$(inherited)');

    settings.SUPPORTS_MACCATALYST = false;
    settings.SUPPORTS_MAC_DESIGNED_FOR_IPHONE_IPAD = false;

    //
    // Libraries search paths
    //
    let iosLinkDirs;
    //TODO:platformTag
    // if (platformTag == "IOS" && (iosLinkDirs = processValue('ios_link_directories'))){
    //     outParamBegin("LIBRARY_SEARCH_PATHS")
    //     outItem("$(inherited)")
    //     outListExpanded(iosLinkDirs)
    //     outParamEnd(True)
    // }

    // Libraries and other linker flags
    let ldFlags = processVar('other_ldflags');
    let linkLibraries = processVar('link_libraries');

    //first collect all the libraries
    //TODO: sources

    project.settings = settings;

    //
    // targets:
    //
    let projectTarget = {
        type: 'application',
        platform: platformName,
        requiresObjCLinking: 'YES',
        settings: {},
    };

    if ((param = processVar('PROJECT_TARGETED_DEVICE_FAMILY'))) {
        projectTarget.settings.base = {};
        projectTarget.settings.base.TARGETED_DEVICE_FAMILY = param;
    }

    projectTarget.settings.configs = {};
    projectTarget.settings.configs['Debug'] = {
        GCC_PREPROCESSOR_DEFINITIONS: [...defines, '_DEBUG=1'],
    };
    projectTarget.settings.configs['AdHocDebug'] = {
        GCC_PREPROCESSOR_DEFINITIONS: [...defines, '_DEBUG=1'],
    };
    projectTarget.settings.configs['Release'] = {
        GCC_PREPROCESSOR_DEFINITIONS: [...defines, '_DEBUG_OPTIMIZED=1'],
    };
    projectTarget.settings.configs['AdHoc'] = {
        GCC_PREPROCESSOR_DEFINITIONS: [...defines],
    };
    projectTarget.settings.configs['AppStore'] = {
        GCC_PREPROCESSOR_DEFINITIONS: [...defines, '_TARGET_APPSTORE=1'],
    };

    //let dependencies = [];
    let dependencies = new Set();

    let frameworks = new Set();

    for (let fw of [
        'AudioToolbox', 'AVFoundation', 'CoreAudio', 
        'CoreMedia', 'MediaPlayer', 
        'OpenAL', 
        'CoreGraphics', 'QuartzCore', 'SystemConfiguration', 'UIKit',
        // Game center
        'Contacts', 'ContactsUI', 'GameKit',

        // AddressBook is obsolete
        'AddressBook', 'AddressBookUI', 'MessageUI',
        'MobileCoreServices', 'Security', 'StoreKit',
    ]) {
        frameworks.add(fw);
    }


    // Multiplayer
    if (processVar('PROJECT_MULTIPLAYER')) {
        for (let fw of [
            'CoreBluetooth', 'MultipeerConnectivity', 'CoreLocation',
        ]) {
            frameworks.add(fw);
        }
    }

    // Multiplayer
    if (processVar('PROJECT_ADS')) {
        for (let fw of [
            'AdSupport', 
            'CoreTelephony', 'EventKit', 'EventKitUI', 'MapKit', 'PassKit', 'SafariServices', 'Social', 'WebKit'
        ]) {
            frameworks.add(fw);
        }
    }

    frameworks.forEach((fw) => {
        //dependencies.push({sdk: `${fw}.framework`});
        dependencies.add({sdk: `${fw}.framework`});
    });

    if ((param = processVar('XCODE_SHARED_FRAMEWORKS'))) {
        for (let fw of param) {
            dependencies.add({sdk: `${fw}.framework`});
        }
    }


        // for (let fw of frameworks) {
        //     dependencies.push({sdk: `${fw}.framework`});
        // }

    if (processVar('PROJECT_ADS')) {
        dependencies.add({sdk: `AppTrackingTransparency.framework`, weak: true});
    }

    if ((param = processVar('XCODE_OPTIONAL_FRAMEWORKS'))) {
        for (let fw of param) {
            dependencies.add({sdk: `${fw}.framework`, weak: true});
        }
    }

    //TODO: for xcode if (param = processVar('XCODE_SIGNED_FRAMEWORKS')) {

    if ((param = processVar('XCODE_SHARED_LIBRARIES'))) {
        for (let fw of param) {
            dependencies.add({sdk: `${fw}.tbd`});
        }
    } else {
        for (let fw of ['libxml2', 'libsqlite3', 'libz']) {
            dependencies.add({sdk: `${fw}.tbd`});
        }
    }


    if (param = processVar('PROJECT_SIGNED_FRAMEWORKS')) {
        for (let fw of param) {
            let ext = getFileExt(fw);
            let suffix = ext ? "" : '.framework';

            dependencies.add({
                framework: `${fw}${suffix}`, 
                codeSign: true, 
                embed: true,
                copy: 'frameworks',
            });
        }
    }




    projectTarget.dependencies = dependencies;


    //
    // Scripts
    //
    let postBuildScripts = [];
    if ((param = processVar('PROJECT_POST_BUILD_SCRIPTS'))) {
        //let scripts = [];

        for (let script of param) {
            //console.log(script);
            
            let scriptContents = ''; //script.script;
            if (Array.isArray(script.script)) {
                //scriptContents = '|';
                for (let line of script.script) {
                    scriptContents += '\n';
                    //scriptContents += '   ';
                    scriptContents += line;
                }
            }

            postBuildScripts.push({
                name: script.name,
                script: scriptContents,
            })
        }


        //console.log(param);
    }

    if (processVar('PROJECT_CRASHLYTICS')) {
        postBuildScripts.push({
            name: 'Crashlytics Upload',
            script: `
if [ "\${CONFIGURATION\}" = "AppStore" ]; then
    echo "Uploading symbols to Crashlytics.."
    Pods/FirebaseCrashlytics/run
fi

`,

            basedOnDependencyAnalysis: false,
            showEnvVars: true,
            inputFiles: [
                '${DWARF_DSYM_FOLDER_PATH}/${DWARF_DSYM_FILE_NAME}',
                '${DWARF_DSYM_FOLDER_PATH}/${DWARF_DSYM_FILE_NAME}/Contents/Resources/DWARF/${PRODUCT_NAME}',
                '${DWARF_DSYM_FOLDER_PATH}/${DWARF_DSYM_FILE_NAME}/Contents/Info.plist',
                '$(TARGET_BUILD_DIR)/$(UNLOCALIZED_RESOURCES_FOLDER_PATH)/GoogleService-Info.plist',
                '$(TARGET_BUILD_DIR)/$(EXECUTABLE_PATH)',
            ],
        })

        //console.log(postBuildScripts);
    }


    if (postBuildScripts.length > 0 )
        projectTarget.postBuildScripts = postBuildScripts;



    let sources = [];

    for (let group of groups) {
        if (!evalIf(group)) continue;

        for (let sourceFile of group.sourceFiles) {
            let dirname = path.dirname(sourceFile);
            //console.log(dirname);

            let groupName = path.join(group.sourceGroupPrefix, group.sourceGroupName);

            if (dirname && dirname != '.')
                groupName = path.join(groupName, dirname);

            let targetSource = {
                path: path.join(group.sourceDir, sourceFile),
                group: groupName,
            };

            if (group.disable_warnings) {
                targetSource.compilerFlags = ["-w"];
            }


            sources.push(targetSource);
        }
    }

    projectTarget.sources = sources;

    project.targets = {};
    project.targets[projectName] = projectTarget;


    let yamlContents = '';

    let genDate = new Date();
    let dateString = genDate.toISOString().split('T')[0];

    yamlContents += `# Automatically Generated *** DO NOT EDIT\n`;
    yamlContents += `# Generated Date: ${dateString}\n`;
    yamlContents += `\n`;

    yamlContents += YAML.stringify(project);

    yamlContents += `\n`;

    fs.writeFileSync(xcodeProjFile, yamlContents);
}
