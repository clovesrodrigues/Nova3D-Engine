# Nova3D Import Format Matrix

| Format | Status | Notes |
|---|---|---|
| OBJ | Official | Covered by regression fixture with structural assertions. |
| FBX | Experimental | Assimp backend wired, but current fixture validates expected failure path; no production FBX golden asset in-repo yet. |
| glTF | Experimental | Backend path is available via Assimp; regression currently validates malformed-file failure only. |
| MD3 | Experimental | Difficult format included in automated expected-failure regression; requires real licensed golden sample to promote. |
| MD5 (.md5mesh/.md5anim/.md5camera) | Experimental | Parser path via Assimp; current tests prove controlled failure handling for malformed assets only. |
| Any other format | Unsupported | Not part of this production acceptance round. |
