// @ts-check

export function toArray(item) {
    if (!item) return [];
    if (Array.isArray(item)) return item;
    return [item];
}

