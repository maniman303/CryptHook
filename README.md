# CryptHook

The project creates a dynamic linked library that is fully independent and manually loads the `BCrypt.dll`.
After that the original `BCryptVerifySignature` method can be hooked, allowing us to bypass the check.

The `CryptHook.dll` placed in the root directory needs to be injected with eg. Koaloader. This should be enough for using the bypass (e.g. for **Frostbite games**).

The default output folder is called `Artifacts`.

Project was made as a solution for games on Linux. WINE and Proton do not have `CryptBase.dll` file.

Based on [CryptBaseBypass](https://github.com/VNNCC/CryptBaseBypass)

# MinHook

The library [MinHook](https://github.com/TsudaKageyu/minhook) is used for hooking the function.

# License

This project is licensed under the [Apache License 2.0](LICENSE).