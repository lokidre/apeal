// @ts-check

import path from 'path/posix';
import fs from 'fs';

let cleanDirs = ['build', '_build', '_build*', 'cmake-build-debug', 'cmake-build-release', '.idea', 'node_modules'];
let cleanDirsRegEx = ['^_build_.*'];

export async function cleanBuild() {
    await clearDir(".");
}

async function clearDir(dirName) {

    //console.log("reading ", dirName);

    //let entries = fs.readdirSync(dirName, {withFileTypes: true});
    let entries = fs.readdirSync(dirName);

    for (let entry of entries) {

        let filePath = path.join(dirName, entry);

       
        let matches = cleanDirs.includes(entry);

        if (!matches) {
            for (let cleanDir of cleanDirsRegEx) {
                let reg = new RegExp(cleanDir);
                let m = reg.test(entry);

                if (m) {
                    matches = true;
                    break;
                }
            }
        }

        if (matches) {
            console.log("deleting: ", filePath);
            fs.rmSync(filePath, {recursive: true, force: true});
            continue;
        }


        let stats = fs.statSync(filePath);

        if (stats.isDirectory()) {
            clearDir(filePath);
        }

        //console.log(filePath, stats.isDirectory());
        //console.log(entry);
    };

    //console.log(entries);
}

//await clearDir(fs.realpathSync('.'));


