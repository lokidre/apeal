import "fs";

export function generateApealSourcePkgConfig(apealDir) {

    const apealVersionFile = apealDir + "/VERSION";

    const apealVersionContents = fs.readFileSync(apealVersionFile).toString();
    const apealVersion = apealVersionContents.split('\n')[0];
    

    let pkgConfig = `prefix=${apealDir}
includedir=\${prefix}/include
rootdir=\${prefix}
srcdir=\${prefix}/src

Name: apeal
Description: Application Library
Version: ${apealVersion}
CFlags: -I\${includedir}
`;

    return pkgConfig;
}
