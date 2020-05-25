![](https://github.com/zqsd/8ssedt/workflows/Node.js%20CI/badge.svg)

Introduction
============
This package generates [2d signed distance field](https://en.wikipedia.org/wiki/Distance_transform).  
Basically it means that from a any 2d array, it calculates the distance to the border between the inside and outside of objects.  

One of the common uses is [glyph rendering on the GPU](https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf). It allows rendering that works well at any resolution with small memory footprint and short GPU time.  

But the textures have to be computed, and it can take some time. The [8ssedt](http://www.lems.brown.edu/vision/people/leymarie/Refs/CompVision/DT/DTpaper.pdf) algorithm is fast and has a O(n) complexity while still providing accurate results.  
It was implemented in C++ to provide fast results, since it's still processing each pixels of a texture.

This implementation was inspired by [this tutorial](https://github.com/Lisapple/8SSEDT/blob/master/README.md).

API
===
There is only one function :

*generateSdf(destination, source, width, height, [options])*

* `destination` <[Uint8Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array)> 2d array containing the result distances
* `source` <[Uint8Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array)> 2d array containing the result distances
* `width` <[number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Data_structures#Number_type)>
* `height` <[number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Data_structures#Number_type)>
* `options` <[Object](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object)>
    * `stride` <[number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Data_structures#Number_type)> number of channels in the array. Defaults to `1`. Set `4` for RGBA images
    * `channel` <[number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Data_structures#Number_type)> channel to read from and write to. Defaults to `0`
    * `threshold` <[number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Data_structures#Number_type)> threshold at which we can consider being inside. Defaults to `≥ 128`
    * `radius` <[number](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Data_structures#Number_type)> radius at which the distance expands. Defaults to `127` pixels inside and outside (so basically a range of 256 to exploit the whole byte), but you most likely want less.

Example
=======

```
const generateSdf = require('8ssedt');

const width = 20, height = 20, size = width * height;
const src = new Uint8Array(size);
const dst = new Uint8Array(size);

for(let y = 0; y < height; y++) {
    for(let x = 0; x < width; x++) {
        src[y * w + x] = y >= 7 && y <= 13 && x >= 5 && x <= 15 ? 255 : 0;
    }
}

generateSdf(dst, src, width, height, {
    stride: 4, // default to 1 for grayscale image, can also be 4 for RGBA
    //channel: 0, // defaults to channel 0
    //threshold: 128, // threshold (including) for delimiting outside and inside
    radius: 1, // length of SDF
});

for(let y = 0; y < height; y++) {
    console.log(dst.slice(y * w, (y + 1) * w).join(d => `${d}\t`));
}
/*

*/
```
