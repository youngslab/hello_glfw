


# How to build

## Download GLFW submodule
```
$ git submodule update --init --recursive
```

## For wayland(xdg_shell)

```
$ mkdir build && cd build
$ cmake -DGLFW_USE_WAYLAND=y -DECM_DIR=/usr/share/ECM/cmake ..
```
