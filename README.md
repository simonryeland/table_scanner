<!-- README.md file -->

# Table OCR Scanner QT6

## build:

```bash
git clone https://github.com/simonryeland/table_scanner.git
cd table_scanner
qmake6 tabscan.pro
make
```

## clear:

```bash
make clean; rm -r build_tmp/ tmp/* tabscan Makefile .qmake.stash
```

<!--
time { file="tabscan"; make clean; qmake6 "${file}.pro" && make && ./${file}; }

git add . && git commit -m "update all files" && git push origin main

git checkout main
git rebase -i --root
git push origin main --force

grep -rin --include="*.h" --include="*.cpp" "btnScan" .
-->

