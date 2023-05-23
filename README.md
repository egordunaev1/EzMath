# EzMath
EzMath - плагин для obsidian.md, позволяющий проводить математические рассчеты, упрощать математические выражения.

Сборка из исходников:
```
cd path_to_your_vault/.obsidian/plugins
git clone https://github.com/egordunaev1/EzMath
cd EzMath
mkdir build-cmake && cd build-cmake && cmake .. && make -j6 && cd ..
npm i
npm run build
```

Требования:
- Node.js
- cmake
- компилятор C++, поддерживающий c++20
