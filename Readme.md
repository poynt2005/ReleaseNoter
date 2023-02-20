# ReleaseNoter

## 如何編譯執行檔

_makefile python include 目錄要改自己的目錄_

~~1. 安裝 Visual Studio 2019 (包含 .NET Framework 4.7.2, CLR Runtime), Python 3.10, MinGW w64, python pip 安裝 cython 2. 編譯 libgit2 (make && make install ) (最好使用 MinGW, MSVC 應該也可以) ， 將 install 目錄下所有檔案拷貝至 ReleaseNoter/libgit2 3. 編譯 libfmt (make && make install ) (MinGW 編譯) ， 將 install 目錄下所有檔案拷貝至 ReleaseNoter/libgit2 4. ReleaseNoter 目錄下，make utility 5. ReleaseNoter 目錄下，make noter 6. ReleaseNoter/csdll/ClipboardCopier 目錄下，開 sln 建置 Debug 專案 7. ReleaseNoter/csdll/ManagedCallBridge 目錄下，開 sln 建置 Debug x64 專案 8. ReleaseNoter/python 目錄下，make noter 9. ReleaseNoter/python 目錄下，make pyd 10. ReleaseNoter/python/pyglober 目錄下，make glober 11. ReleaseNoter/python/gui 目錄下，make util 12. ReleaseNoter/python/gui 目錄下，make pyd 13. ReleaseNoter/python/gui 目錄下，make main 14. ReleaseNoter/utilities/PyEmbedDownloader 目錄下，make util 15. ReleaseNoter/utilities/TclTkPycMaker 目錄下，make test 16. ReleaseNoter/utilities/TclTkPycMaker/c++cli 目錄下，開 sln 選 x64 建置 debug 17. 將 ReleaseNoter/utilities/TclTkPycMaker/c++cli/x64/Debug/TclTkPycMaker.dll 複製到 ReleaseNoter/utilities/TclTkPycMaker/ 資料夾中，將 ReleaseNoter/python/pyglober/pyglober.dll 複製到 ReleaseNoter/utilities/TclTkPycMaker/ 資料夾中，運行 test.exe 18. 運行 python ReleaseNoter/utilities/PyEmbedDownloader/PyEmbedDownloader.py，將~~
太複雜了不寫了

總之先個別編譯  
然後把結果放一起
