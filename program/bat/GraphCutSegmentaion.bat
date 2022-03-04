@ECHO OFF

rem remから始まる行はコメント
rem 最後のMASKFILENAMEはオプション
rem 結果がおかしかったり落ちたりしたら，common.hのPixelTypeが画像ファイルのとあっているかを確認
rem あっていればプログラムのバグですm(__)m。頑張って直してください

rem exeファイルへのパス
set EXE=..\GraphCutSegmentation\x64\Release\GraphCutSegmentation.exe

set ROOT=data

rem 入力画像へのパス
set INPUT=input

rem シード画像へのパス
set SEED=seed

rem 出力画像へのパス
set OUTPUT=output/sigma55ambda1

rem マスク画像へのパス
set MASK=mask

set SIGMA=5.5
set LAMBDA=1

%EXE% %FILENAME% --sigma %SIGMA% --lambda %LAMBDA% -r %ROOT% -o %OUTPUT%