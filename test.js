const test = require('ava');
const generateSdf = require('./index');

/* 0 1 2 3 4
0  . . . . .
1  . . . . .
2  . . o o .
3  . . o . .
4  . . . . .
*/
const src_5x5 = new Uint8Array(5 * 5 * 4);
for (let i = 0; i < src_5x5.length; i++)
    src_5x5[i] = 0;
src_5x5[4 * (2 * 5 + 2)] = 255;
src_5x5[4 * (3 * 5 + 2)] = 255;
src_5x5[4 * (2 * 5 + 3)] = 255;

function arrayToString(dst, w, h, stride = 4, channel = 0) {
    return [...Array(h).keys()].map(y => {
        return [...Array(w).keys()].map(x => {
            return String(dst[stride * (y * w + x) + channel] - 127).padStart(4, ' ');
        });
    }).join('\n');
}

test('5x5 rgba', async t => {
    const dst = new Uint8Array(src_5x5.length);

    generateSdf(dst, src_5x5, {
        width: 5,
        height: 5,
        stride: 4
    });

    t.is(t.is(arrayToString(dst, 5, 5), `   3,   2,   2,   2,   2
   2,   1,   1,   1,   1
   2,   1,   0,   0,   1
   2,   1,   0,   1,   1
   2,   1,   1,   1,   2`));
});

test('short radius', async t => {
    const dst = new Uint8Array(src_5x5.length);

    generateSdf(dst, src_5x5, {
        width: 5,
        height: 5,
        stride: 4,
        radius: 2,
    });

    t.is(t.is(arrayToString(dst, 5, 5), ` 128, 128, 127, 127, 128
 128,  90,  64,  64,  90
 127,  64,   0,   0,  64
 127,  64,   0,  64,  90
 128,  90,  64,  90, 128`));
});

test('big box', async t => {

    const w = 10, h = 10, size = w * h;
    const src = new Uint8Array(size);
    const dst = new Uint8Array(size);
    for (let y = 0; y < h; y++) {
        for (let x = 0; x < w; x++) {
            src[y * w + x] = y >= 2 && y <= 7 && x >= 3 && x <= 6 ? 255 : 0;
        }
    }

    generateSdf(dst, src, {
        width: w,
        height: h,
    });

    t.is(arrayToString(dst, w, h, 1), `   4,   3,   2,   2,   2,   2,   2,   2,   3,   4
   3,   2,   1,   1,   1,   1,   1,   1,   2,   3
   3,   2,   1,   0,   0,   0,   0,   1,   2,   3
   3,   2,   1,   0,  -1,  -1,   0,   1,   2,   3
   3,   2,   1,   0,  -1,  -1,   0,   1,   2,   3
   3,   2,   1,   0,  -1,  -1,   0,   1,   2,   3
   3,   2,   1,   0,  -1,  -1,   0,   1,   2,   3
   3,   2,   1,   0,   0,   0,   0,   1,   2,   3
   3,   2,   1,   1,   1,   1,   1,   1,   2,   3
   4,   3,   2,   2,   2,   2,   2,   2,   3,   4`);
});