
out vec4 sk_FragColor;
in vec4 src;
in vec4 dst;
vec4 blend_src_in(vec4 src, vec4 dst) {
    return src == vec4(0.0) ? vec4(0.0) : src * dst.w;
}
vec4 blend_dst_in(vec4 src, vec4 dst) {
    vec4 _2_blend_src_in;
    {
        _2_blend_src_in = dst == vec4(0.0) ? vec4(0.0) : dst * src.w;
    }
    return _2_blend_src_in;

}
void main() {
    vec4 _0_blend_src_in;
    {
        _0_blend_src_in = src == vec4(0.0) ? vec4(0.0) : src * dst.w;
    }

    sk_FragColor = _0_blend_src_in;

    vec4 _1_blend_dst_in;
    {
        vec4 _3_blend_src_in;
        {
            _3_blend_src_in = dst == vec4(0.0) ? vec4(0.0) : dst * src.w;
        }
        _1_blend_dst_in = _3_blend_src_in;

    }

    sk_FragColor = _1_blend_dst_in;

}
