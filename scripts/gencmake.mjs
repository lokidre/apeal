#! /usr/bin/env zx

import {generate} from './genproj/index.mjs';

let params = {
    buildConfig : '../.build-config.yaml',
    sources : './apeal_sources.yaml',
    targetCmakeSources : './cmake/apeal_sources.cmake',
}

generate(params);

