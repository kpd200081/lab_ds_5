./lab_ds_5 --compress --input_filename=orig.txt --output_filename=comp.txt --huff
./lab_ds_5 --decompress --input_filename=comp.txt --output_filename=res.txt --huff
./lab_ds_5 --compress --input_filename=orig.txt --output_filename=comp.txt --lzw
./lab_ds_5 --decompress --input_filename=comp.txt --output_filename=res.txt --lzw

./lab_ds_5 --compress --input_filename=orig.txt --output_filename=comp.txt --huff --lzw
./lab_ds_5 --decompress --input_filename=comp.txt --output_filename=res.txt --lzw --huff
./lab_ds_5 --compress --input_filename=orig.txt --output_filename=comp.txt --lzw --huff
./lab_ds_5 --decompress --input_filename=comp.txt --output_filename=res.txt --huff --lzw

