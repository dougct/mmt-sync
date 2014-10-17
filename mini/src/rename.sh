for file in *
do
  mv "$file" "$file.cpp" #${file%%.*}
done
