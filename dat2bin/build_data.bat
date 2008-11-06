@echo 開始建造新酷音輸入法詞庫檔...
dat2bin.exe
del *.dat_src
ren ch_index.dat *.dat_src
ren ph_index.dat *.dat_src
ren fonetree.dat *.dat_src
ren *.dat_bin *.dat
pause