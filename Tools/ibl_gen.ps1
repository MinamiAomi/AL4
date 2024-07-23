$input_path = Read-Host "入力ファイル"
$output_directory = Read-Host "出力ディレクトリ"
$size = [int](Read-Host "出力サイズ")
$input_name = [System.IO.Path]::GetFileNameWithoutExtension("$input_path")
$output_path = Join-Path -Path $output_directory -ChildPath $input_name
New-Item $output_path -ItemType "directory"
$output_name = Join-Path -Path $output_path -ChildPath $input_name
.\cmft.exe --input $input_path --dstFaceSize $size --outputNum 1 --output0 $output_name --output0params dds,rgba16f,cubemap
# .\cmft.exe --input $input_path --filter irradiance --dstFaceSize $size --mipCount 1 --outputNum 1 --output0 "${output_name}_irradiance" --output0params dds,rgba16f,cubemap
.\cmft.exe --input $input_path --filter radiance --useOpenCL true --dstFaceSize $size --mipCount 11 --outputNum 1 --output0 "${output_name}_radiance" --output0params dds,rgba16f,cubemap
.\texconv.exe -y -o $output_path -f BC6H_UF16 "${output_name}.dds"
# .\texconv.exe -y -o $output_path -f BC6H_UF16 "${output_name}_irradiance.dds"
.\texconv.exe -y -o $output_path -f BC6H_UF16 "${output_name}_radiance.dds"
Write-Host "${output_path}に生成しました。"
pause