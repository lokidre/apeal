// @ts-check

// @ts-ignore
import fs from 'fs';

// @ts-ignore
import path from 'path/posix';

import YAML from '../js/yaml.mjs';

/** @param {string} p - path */
function getPath(p) {
    return path.resolve(p).replaceAll('\\', '/');
}

/** @param {string} p - path */
function extractDir(p) {
    return path.dirname(p);
}

export function loadProjectParams(genproj, params) {
    params.set('GENPROJ_BUILD_CONFIG_DIR', extractDir(getPath(genproj.buildConfig)));

    if (genproj.projectConfig) {
        params.set('GENPROJ_PROJECT_CONFIG_DIR', extractDir(getPath(genproj.projectConfig)));
    }   
}


export function loadConfigParams(yamlFile, configParams) {
    const doc = YAML.parse(fs.readFileSync(yamlFile, 'utf8'));
    let prefix = '';

    for (const [object, value] of Object.entries(doc)) {
        if (object === 'config') {
            prefix = '';
        } else {
            prefix = 'CONFIG_';
        }
        //const tempVars = new Map();
        const tempVars = new Map(configParams);

        for (const [key, val] of Object.entries(value)) {
            tempVars.set(key.toUpperCase(), val);

            let varName = `${prefix}${object}_${key}`.toUpperCase();

            if (typeof val === 'object') {
                let newArr = [];

                Object.entries(val).forEach(([key, value]) => {
                    newArr.push(processValueBody(value, tempVars));
                });

                configParams.set(varName, newArr);
            } else {
                configParams.set(varName, processValueBody(val, tempVars));
            }
        }
    }

    //console.log(configParams);
    //return configParams;
}

// export function processValue(value) {
//     return processValueBody(value, configParams);
// }

export function processValueBody(value, vars, nothrow) {

    if (Array.isArray(value)) {
        let values = [];
        for (let v of value) {
            values.push(processValueBody(v, vars, nothrow));
        }
        return values;
    }

    if (typeof value === 'number') return value.toString();


    if (typeof value === 'object') {
        let object = {};
        for (let v in value) {
            object[v] = processValueBody(value[v], vars, nothrow);
        }
        return object;
    }


    // if (typeof value !== 'string')
    // console.log(typeof value);

    // Skip CMAKE_ variables
    if (value.startsWith("${CMAKE_")) {
        return value;
    }


    let regex = new RegExp(/\$\{[a-zA-Z0-9_.-]*\}/gi);
    let matches = value.match(regex, value);

    if (!matches) return value;

    let replaced = false;

    // TODO: harcoding xcode variables
    let xcodeBuiltinVars = ['BUILT_PRODUCTS_DIR', 'FRAMEWORKS_FOLDER_PATH'];


    for (let match of matches) {
        if (match.length <= 3) continue;

        let varName = match.substring(2, match.length - 1);

        if (xcodeBuiltinVars.includes(varName))
            continue;

        let varValue = vars.get(varName.toUpperCase());

        if (varValue === undefined) {
            if (nothrow)
                return undefined;
            throw new Error(`Invalid variable: ${varName}`);
        }

        replaced = true;

        if (Array.isArray(varValue)) {
            value = varValue;
        } else {
            value = value.replace(match, varValue);
            //console.log(match, varValue, value);
        }

        // if (varName === 'CONFIG_INCLUDE_DIRECTORIES') {
        //     console.log(varValue, value);
        // }
    }


    if (replaced) return processValueBody(value, vars);

    return value;
}
