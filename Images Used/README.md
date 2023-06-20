# Images Used

The images which were used in evaluating the performance of both the Mean Shift Algorithm and Otsu's Method in the Bachelor's Thesis.

All segmentation masks were initially saved using the TIF-format, but were later converted to PNG to allow for previewing on GitHub, using the [following command](https://askubuntu.com/a/60410):

```
sudo apt install imagemagick
find -name *.tif | while read f; do echo "Converting $f to $(dirname "$f")/$(basename "$f" .tif).png"; convert "$f"  "$(dirname "$f")/$(basename "$f" .tif).png"; rm "$f"; done
```

[Input](./Input) contains the original and binarised expression masks, and the raw ISH-stained images from the Allen Mouse Brain Atlas for both the training and test data sets.

[Output](./Output) contains the produced segmentation masks by both the Mean Shift Algorithm and Otsu's Method for each raw image in Input.