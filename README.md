# stripcert

**Purpose:** This utility strips the AuthentiCode (code-signing) signature from a PE binary.
**NB:** Beware that it will strip anything beyond the so-called security directory. That means if an overlay has been added past the signature, it too will get stripped.

This is a small utility which I wrote back in 2007 during my first month in Iceland. I created it with DDKWizard at the time, albeit obviously an older version of DDKWizard.

I never published it, but the topic came up in 2021 and now again in 2023. So I took the time to run `clang-format` on it, fix the few code analysis warnings and wrap it up in a DDKWizard-generated VS2005 project from the latest DDKWizard version as well as a premake4-generated VS2022 project.

The VS2005 solution/project targets the Windows 7 SP1 WDK (7600.16385.1), but should be possible to target even further back; this WDK supports only Windows back until XP.

PS: these days [osslsigncode](https://github.com/mtrojnar/osslsigncode) has a command to achieve the same.
