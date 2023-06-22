# Software documentation

To use our software you need to install bazel to do that following each steps of this link for macOs:
https://bazel.build/install/os-x.html

or this link for windows : 
https://docs.bazel.build/versions/master/install-windows.html

or this link for Linux : 
https://docs.bazel.build/versions/master/install-ubuntu.html

Then you can clone our repository change into the blend.cpp file the path line 117 to find the output.txt file for that you need to copy paste the path where you have clone the Github and change it in the line 117 after that you add /output.txt and now. You can use Windows, MacOs or Linux to run this software with this following command after install bazel.

```
bazel build :blend
bazel run :blend
```
