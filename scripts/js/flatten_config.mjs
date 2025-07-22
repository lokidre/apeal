// @ts-check

/**
 * @param {{ [x: string]: any; }} obj
 */

export function flattenConfig (obj, prefix = '') {
    return Object.keys(obj).reduce((acc, k) => {
        const pre = prefix.length ? prefix + '_' : '';

        if (Array.isArray(obj[k])) {
            acc[pre + k] = obj[k].join(';');
            return acc;
        }

        if (typeof obj[k] === 'object') {
            Object.assign(acc, flattenConfig(obj[k], pre + k));
            return acc;
        } 

        acc[pre + k] = obj[k];

        return acc;
    }, {});
}

