// `export * as default from ...` fails on Webpack v4
// https://github.com/eemeli/yaml/issues/228
import * as YAML from './yaml/index.mjs';
export default YAML;
export * from './yaml/index.mjs';
