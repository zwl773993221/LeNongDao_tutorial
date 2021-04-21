/^# Packages using this file: / {
  s/# Packages using this file://
  ta
  :a
  s/ e2fsprogs / e2fsprogs /
  tb
  s/ $/ e2fsprogs /
  :b
  s/^/# Packages using this file:/
}
