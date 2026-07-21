import sys
import pathlib
t = pathlib.Path("core/interception_wrapper.c").read_text("utf-8")
old = "interception_key_stroke_t stroke;\n    stroke.code"
new = "uint8_t stroke_buf[16] = {0};\n    interception_key_stroke_t *ks = (interception_key_stroke_t *)stroke_buf;\n    ks->code"
t = t.replace(old, new)
old2 = "stroke.state       = state;\n    stroke.information = 0;\n\n    // 扩展键标志\n    if ((scan >> 8) == 0xE0)\n        stroke.state |= INTCP_KEY_E0;\n\n    int ret = s_send(s_ctx, s_dev, &stroke, 1);"
new2 = "ks->state       = state;\n    ks->information = 0;\n    if ((scan >> 8) == 0xE0)\n        ks->state |= INTCP_KEY_E0;\n    int ret = s_send(s_ctx, s_dev, stroke_buf, 1);"
t = t.replace(old2, new2)
pathlib.Path("core/interception_wrapper.c").write_text(t, "utf-8")
print("fix done")
