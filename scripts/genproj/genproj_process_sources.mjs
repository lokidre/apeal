// @ts-check

import fs from 'fs';

import {processValueBody} from './genproj_config.mjs';

import {toArray} from '../js/to_array.mjs';
import {getFilesFromPath} from './genproj_tools.mjs';

export function genprojProcessSources(sources, configParams) {
    let sourcesData = {
        needPkgConfig: false,
        groups: new Array(),
    };

    //let sourceGroups = [];

    //
    // First pass, analyze source groups
    //
    for (let source of sources) {
        let group = {
            // From yaml file
            files: toArray(source.files),
            mask: toArray(source.mask),

            dir: source.dir,

            if: source.if,
            compile_definitions: toArray(source.compile_definitions),
            compile_options: source.compile_options,
            disable_warnings: source.disable_warnings,
            disable_pch: source.disable_pch,

            link_libraries: toArray(source.link_libraries),
            link_directories: toArray(source.link_directories),
            link_debug_postfix: source.link_debug_postfix,

            bundle: source.bundle,
            prefix: source.prefix,
            group_base: source.group_base,
            source_group: source.group ?? source.source_group,
            resource_group: source.resource_group,
            bundle_group: source.bundle_group,
            frameworks_group: source.frameworks_group,
            exclude_directories: toArray(source.exclude_directories),
            exclude_pattern: toArray(source.exclude_pattern),
            include_directories: toArray(source.include_directories),
            packages: toArray(source.packages),

            // Generated
            sourceDir: source.dir ?? '.',

            sourceGroupTree: true,
            sourceGroupPrefix: 'Project',
            sourceGroupName: '',
            //sourceFiles: [],
            sourceFiles: new Set(),
            recursive: source.recursive === undefined ? true : source.recursive,
        };

        if (group.dir) {
            group.sourceDir = processValueBody(group.dir, configParams);

            // Skip directory checks for CMAKE_ variables
            let skipCheck = group.sourceDir.includes("${CMAKE_");

            if (!skipCheck && !fs.existsSync(group.sourceDir)) {
                throw new Error(`Error: Directory doesn't exist: ${group.sourceDir}`);
            }
        }

        if (group.source_group) {
            group.sourceGroupPrefix = group.group_base ?? 'Sources';
            group.sourceGroupName = group.source_group;

            if (group.files.length === 0 && group.mask.length === 0) {
                group.mask = ["*.cpp", "*.c", "*.cc", "*.hpp", "*.h", "*.hh"];
            }
        }

        if (group.resource_group) {
            group.sourceGroupPrefix = group.group_base ?? 'Resources';
            group.sourceGroupName = group.resource_group;
        }
        if (group.bundle_group) {
            group.sourceGroupPrefix = group.group_base ?? 'Bundle';
            group.sourceGroupName = group.bundle_group;

            if (group.files.length === 0 && group.mask.length === 0) {
                switch (group.bundle_group.toLowerCase()) {
                case "music":  group.mask = ["*.mp3"]; break;
                case "sounds":  
                case "sfx":  group.mask = ["*.flac"]; break;
                case "texture":  
                case "textures": group.mask = ["*.yaml", "*.png"]; break;
                case "schema":
                case "scheme": group.mask = ["*.yaml", "*.yml", "*.ini", "*.csv"]; break;
                default: break;
                }
            }
        }
        if (group.frameworks_group) {
            group.sourceGroupPrefix = group.group_base ?? 'Frameworks';
            group.sourceGroupName = group.frameworks_group;
        }

        for (let filename of group.files) {
            group.sourceFiles.add(filename);
        }

        let files = getFilesFromPath(group.sourceDir, group);

        for (let file of files) {
            group.sourceFiles.add(file);
        }

        if (group.packages.length > 0) {
            sourcesData.needPkgConfig = true;
        }

        sourcesData.groups.push(group);
    }

    return sourcesData;
}
