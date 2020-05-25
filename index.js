const testAddon = require('./build/Release/testaddon.node');

/**
 * 
 * @param {(Uint8Array|ImageData|Canvas)} dst - array or image containing the distance field result
 * @param {(Uint8Array|ImageData|Canvas)} src - array or image continaing the distance field source
 * @param {Object} [params] - parameters
 * @param {Number} [params.width] - width in pixels, for Uint8array
 * @param {Number} [params.height] - height in pixels, for Uint8array
 * @param {Number} [params.stride=1] - number of channels. 1 for grayscale, 4 for RGBA
 * @param {Number} [params.channel=0] - channel to generate the distance field
 * @param {Number} [params.radius=127] - length of SDF
 * @param {Number} [params.threshold=128] - limit between inside and outside
 */
function generate(dst, src, params = {}) {
    let {width, height} = params;

    let dst_array = dst;
    let dst_ctx, dst_imageData;
    if(dst.constructor.name === 'ImageData') {
        dst_array = dst.data;
        width = dst.width;
        height = dst.height;
        if(!params.stride)
            params.stride = 4;
    }
    else if(dst.constructor.name === 'Canvas') {
        dst_ctx = dst.getContext('2d');
        dst_imageData = dst_ctx.getImageData(0, 0, dst.width, dst.height);
        dst_array = dst_imageData.data;
        width = dst.width;
        height = dst.height;
        if(!params.stride)
            params.stride = 4;
    }

    let src_array = src;
    if(src.constructor.name === 'ImageData') {
        src_array = src.data;
        if(src.width != width || src.height != height) {
            throw new Error('dst and src dimensions do not match');
        }
        if(!params.stride)
            params.stride = 4;
    }
    else if(src.constructor.name === 'Canvas') {
        src_array = src.getContext('2d').getImageData(0, 0, src.width, src.height).data;
        if(src.width != width || src.height != height) {
            throw new Error('dst and src dimensions do not match');
        }
        if(!params.stride)
            params.stride = 4;
    }

    if(!width || !height) {
        throw new Error("Invalid dimensions");   
    }

    testAddon.generate(dst_array, src_array, width, height, params);

    if(dst_ctx && dst_imageData) {
        dst_ctx.putImageData(dst_imageData, 0, 0);
    }
}

module.exports = generate;