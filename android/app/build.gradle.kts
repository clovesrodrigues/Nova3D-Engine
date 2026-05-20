plugins {
    id("com.android.application")
}

android {
    namespace = "com.nova3d.sandbox"
    compileSdk = 34

    defaultConfig {
        minSdk = 24
        targetSdk = 34
        ndk {
            abiFilters += listOf("arm64-v8a")
        }
        externalNativeBuild {
            cmake {
                arguments += listOf("-DANDROID_STL=c++_shared")
            }
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
        }
    }
}
