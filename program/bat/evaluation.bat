@ECHO OFF

set EXE=..\GraphCutSegmentation\x64\Release\evaluation.exe

rem 評価用画像(FPとかFNとか)を出力するフォルダ
set OUTPUT=data/evaluate

rem セグメンテーション結果へのパス
set RESULT=result.mhd

rem 正解へのパス
set ANSWER=gt.mhd

rem マスクへのパス(オプション，指定しなければ画像全体に適用)
set MASK=mask.mhd

%EXE% %OUTPUT% %RESULT% %ANSWER% %MASK% 