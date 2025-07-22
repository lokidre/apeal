// @ts-check

import fs from 'fs';

import {flattenConfig} from './flatten_config.mjs';

import YAML from './yaml.mjs';

/**
 * @param {{ config: any; }} config
 */
export function configureGenerate(config) {
    let yaml = YAML.stringify(config, {
        collectionStyle: 'block',
        lineWidth: 0,
    });

    if (yaml) {
        fs.writeFileSync('.build-config.yaml', yaml);
    }

    fs.writeFileSync('.build-config.json', JSON.stringify(config, null, 2));

    let flat = flattenConfig(config);

    let js = '// Automatically generated\n';
    js += 'export const config = ';
    js += JSON.stringify(config.config, null, 2);
    js += ';\n';

    fs.writeFileSync('.build-config.mjs', js);

    
    let cmake = '# Automatically generated\n';
    for (let key of Object.keys(flat)) {
        cmake += `set(${key.toUpperCase()} \"${flat[key]}\")\n`;
    }

    fs.writeFileSync('.build-config.cmake', cmake);



    let ninja = '# Automatically generated\n';
    for (let key of Object.keys(flat)) {
        ninja += `${key.toUpperCase()} = ${flat[key]}\n`;
    }

    fs.writeFileSync('.build-config.ninja', ninja);


    let make = '# Automatically generated\n';
    for (let key of Object.keys(flat)) {
         make += `${key.toUpperCase()} := ${flat[key]}\n`;
    }
    fs.writeFileSync('.build-config.make', make);

}
