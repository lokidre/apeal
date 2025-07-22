// @ts-check

import {loadConfigParams, loadProjectParams} from './genproj_config.mjs';
import {generateCmake, generateCmakeConfigParams} from './genproj_cmake.mjs';
import {generateXcodeGen} from './genproj_xcodegen.mjs';

/**
 * @param {{ buildConfig: any; targetCmakeConfig: any; targetCmakeSources: any; sources: any; targetXcodeGen: any; }} genParams
 */
export function generate(genParams) {
    let configParams = new Map();
    
    loadProjectParams(genParams, configParams);
    loadConfigParams(genParams.buildConfig, configParams);

    if (genParams.targetCmakeConfig) {
        generateCmakeConfigParams(genParams.targetCmakeConfig, configParams);
    }

    if (genParams.targetCmakeSources) {
        generateCmake(genParams.sources, genParams.targetCmakeSources, configParams);
    }

    if (genParams.targetXcodeGen) {
        generateXcodeGen(genParams, configParams);
    }

}
