// @ts-check

// @ts-ignore: node imports
import fs from 'fs';

import YAML from '../js/yaml.mjs';

import {genprojProcessSources} from './genproj_process_sources.mjs';

export function generateCmake(yaml, targetCmakeSources, configParams) 
{
    const doc = YAML.parse(fs.readFileSync(yaml, 'utf8'));
    let sourcesData = genprojProcessSources(doc.sources, configParams);

    //
    // Cmake rendering
    let indentIndex = 0;
    let indentPrefix = ''
    let cmake = '';

    let makeIndentPrefix = () => {
        indentPrefix = '';
        for (let i = 0; i < indentIndex; ++i) {
            indentPrefix += '  ';
        }
    }

    let indent = () => {
        indentIndex++;
        makeIndentPrefix();
    }

    let unindent = () => {
        if (indentIndex > 0)
            indentIndex--;
        makeIndentPrefix();
    }

    let outn = (s) => {
        cmake += indentPrefix + s + '\n';
    }
    let nl = () => {
        cmake += '\n';
    }


    let genDate = new Date();
    let dateString = genDate.toISOString().split('T')[0];

    outn(`# Automatically Generated *** DO NOT EDIT`);
    outn(`# Generated Date: ${dateString}`);
    nl();



    //
    // Second pass: building cmake
    //
    for (let group of sourcesData.groups) {
        let hasSources = group.sourceFiles.size > 0;

        outn(`# Source Group: ${group.sourceGroupName ?? ''}`)

        if (group.if) {
            outn(`if (${group.if})`);
            indent();
        }

        for (let item of group.compile_definitions) {
            outn(`add_compile_definitions(${item})`);
        }

        for (let item of group.include_directories) {
            outn(`include_directories(${item})`);
        }

        for (let item of group.link_libraries) {
            if (group.link_debug_postfix) {
                outn(`link_libraries(debug ${item}${group.link_debug_postfix} optimized ${item})`);
            } else {
                outn(`link_libraries(debug ${item})`);
            }
        }

        if (group.packages.length > 0) {
            let wasPkgConfig = false;


            for (let pkg of group.packages) {
                if (pkg.libs) {
                    if (!wasPkgConfig) {
                        outn(`find_package(PkgConfig REQUIRED)`);
                        wasPkgConfig = true;
                    }

                    outn(`pkg_check_modules(${pkg.name} REQUIRED ${pkg.libs})`);
                } else {
                    outn(`find_package(${pkg.name} REQUIRED)`);
                }

                outn(`include_directories(\$\{${pkg.name}_INCLUDE_DIRS\})`);
                outn(`link_directories(\$\{${pkg.name}_LIBRARY_DIRS\})`);
                outn(`link_libraries(\$\{${pkg.name}_LIBRARIES\})`);
            }
        }

        if (hasSources) {
            outn(`set(src`);

            indent();
            for (let file of group.sourceFiles) {
                let base = group.prefix ?? group.dir;
                outn(`${base}/${file}`);
            }
            unindent();

            outn(')');

            let sep = '\\\\';
            let vsName = group.sourceGroupPrefix;
            if (group.sourceGroupName) {
                vsName = `${vsName}/${group.sourceGroupName}`;
            }

            vsName = vsName.replace('/', sep);

            if (group.sourceGroupTree) {
                let base = group.prefix ?? group.dir;

                outn(`source_group(TREE ${base} PREFIX \"${vsName}\" FILES \${src})`);
            } else {
                outn(`source_group(${vsName} \${src})`);
            }

            if (group.bundle) {
                outn(`list(APPEND bundle_SRC \${src})`);
            } else {
                outn(`list(APPEND main_SRC \${src})`);
            }

            if (group.disable_warnings) {
                outn(`
if (MSVC)
    set_source_files_properties(\${src} PROPERTIES COMPILE_FLAGS \"/W0\")
else()
    set_source_files_properties(\${src} PROPERTIES COMPILE_FLAGS \"-w\")
endif()
`);
            }

            if (group.disable_pch) {
                outn(`set_source_files_properties(\${src} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)`);
            }
        }

        if (group.if) {
            unindent();
            outn('endif()');
        }

        nl();

    }

    fs.writeFileSync(targetCmakeSources, cmake);
}

export function generateCmakeConfigParams(targetCmakeConfig, configParams) {

    let cmake = '';
    for (let [key, value] of configParams) {
        if (Array.isArray(value)) {
            cmake += `set(${key} "${value.join(';')}")\n`;
        } else {
            cmake += `set(${key} "${value}")\n`;
        }
    }

    fs.writeFileSync(targetCmakeConfig, cmake);
}
