// @ts-check

import fs from 'fs';
import path from 'path/posix';

import {globToRegEx} from '../js/glob_to_regex.mjs';

/**
 * @param {string} firstDir
 * @param {string} secondDir
 */
export function isDirectoryEqualTo(firstDir, secondDir) {
    return path.resolve(firstDir) === path.resolve(secondDir);
}

/**
 * @param {string} filename
 */
export function getFileExt(filename) {
    let comps = filename.split('/');
    let name = comps[comps.length - 1];
    let ncomps = name.split('.');
    if (ncomps.length > 1)
        return ncomps[ncomps.length - 1];
    return null;
}

/**
 * @param {any} dir
 * @param {any} group
 */
export function getFilesFromPath(dir, group) {
    let filenames = [];
    let regexes = [];

    let dirParent = dir;
    // let recursive = group.recursive; // === undefined ? true : group.recursive;

    // if (recursive === undefined)
    //     recursive = true;

    if (Array.isArray(group.mask)) {
        for (let mask of group.mask) {
            regexes.push(globToRegEx(mask));
        }
    } else {
        regexes.push(globToRegEx(group.mask));
    }


    function processPath(p) {
        let files = fs.readdirSync(p, {withFileTypes: true});

        for (let file of files) {
            let filePath = path.join(p, file.name);

            //if (!file.isFile()) {
            if (file.isDirectory()) {

                if (!group.recursive)
                    continue;
                
                let dirExcluded = false;

                if (group.exclude_directories !== undefined && group.exclude_directories.length > 0) {

                    let filePathDir = filePath.substring(dirParent.length + 1);

                    for (let excludeDir of group.exclude_directories) {
                        let regex = globToRegEx(excludeDir);
                        if (filePathDir.match(regex)) {
                            dirExcluded = true;
                            break;
                        }
                    }
                }

                if (!dirExcluded) {
                    processPath(filePath);
                }

                continue;
            }  // if directory

            let fileExcluded = false;
            
            if (group.exclude_pattern) {
                for(let pattExcluded of group.exclude_pattern){
                    let unixPath = filePath.replaceAll('\\', '/');
                    let finalName = p !== '.' ? unixPath.substring(dir.length + 1) : unixPath;
                    if(finalName.match(globToRegEx(pattExcluded))){
                    //if(finalName.match(pattExcluded)){
                        fileExcluded = true;
                        break;
                    }
                }
            }

            if (fileExcluded)
                continue;

            for (let re of regexes) {
                if (file.name.match(re)) {
                    //console.log(file.name);

                    let unixPath = filePath.replaceAll('\\', '/');
                    let finalName = p !== '.' ? unixPath.substring(dir.length + 1) : unixPath;
                    filenames.push(finalName);
                    break;
                }
            }
        }  // for all files
    }  // processPath function

    processPath(dir);
    return filenames;
}
