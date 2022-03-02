@ECHO OFF

rem remから始まる行はコメント
rem 最後のMASKFILENAMEはオプション
rem 結果がおかしかったり落ちたりしたら，common.hのPixelTypeが画像ファイルのとあっているかを確認
rem あっていればプログラムのバグですm(__)m。頑張って直してください

rem exeファイルへのパス
set EXE=..\GraphCutSegmentation\x64\Release\GraphCutSegmentation.exe

rem 入力画像へのパス
set INPUTFILENAME=C:\Users\tanabe\Desktop\graph-cut-segmentation-practice\data\orgSTD_121.mhd

rem シード画像へのパス
set SEEDFILENAME=C:\Users\tanabe\Desktop\graph-cut-segmentation-practice\data\seed_pic.mhd

rem 出力画像へのパス
set OUTPUTFILENAME=C:\Users\tanabe\Desktop\output3.mhd

rem ハイパーパラメータ（sigma）
set SIGMA=1

rem ハイパーパラメータ（lambda）
set LAMBDA=0.001

rem マスク画像へのパス（指定しなければ画像全体で計算）
rem set MASKFILENAME=

%EXE% %INPUTFILENAME% %SEEDFILENAME% %OUTPUTFILENAME% %SIGMA% %LAMBDA%

PAUSE
