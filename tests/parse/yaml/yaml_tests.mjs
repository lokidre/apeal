#! /usr/bin/env zx

import YAML from 'yaml';
import fs from 'fs';
import path from 'path/posix';
import {exit} from 'process';

let testDir = '_tests';
let dataDir = 'data';

let genBin = '_build/yaml_gen';

let testFiles = ['seq_quoted.yaml'];
let skipFiles = ['tag_type.yaml'];


testFiles = [];
let dirEntries = fs.readdirSync(dataDir);

dirEntries.forEach((file) => {
    if (skipFiles.includes(file))
        return;
    if (path.extname(file) == '.yaml') testFiles.push(file);
});

fs.mkdirSync(testDir, {recursive: true});

for (let testFile of testFiles) {
    console.log(testFile);

    let fileDataPath = path.join(dataDir, testFile);
    let fileTestPath = path.join(testDir, testFile);

    let orgYaml = YAML.parse(fs.readFileSync(fileDataPath, 'utf-8'));

    if (orgYaml == null)
        continue;

   
    //echo`${testFile}`

    await $`${genBin} ${fileDataPath} > ${fileTestPath}`;

    let genYaml = YAML.parse(fs.readFileSync(fileTestPath, 'utf-8'));

    //console.log(orgYaml, genYaml);


    if (!isEqual(orgYaml, genYaml)) {
        echo`Yaml don't match: ${testFile}`;
        exit(1);
    }
}

function isEqual(obj1, obj2) {
    //console.log(obj1, obj2);

    let keys1 = Object.keys(obj1);
    let keys2 = Object.keys(obj2);

    if (keys1.length !== keys2.length) return false;

    for (let key of keys1) {
        let val1 = obj1[key];
        let val2 = obj2[key];

        let isObj1 = isObject(val1);
        let isObj2 = isObject(val1);

        //console.log(val1, val2);

        if (isObj1 != isObj2) return false;

        if (isObj1) {
            if (!isEqual(val1, val2)) return false;
        } else {
            if (val1 !== val2) {
                if (toString(val1) !== toString(val2))
                    return false;
            }
        }
    }

    return true;
}

function isObject(object) {
    return object != null && typeof object === 'object';
}
