# Images Used

The images which were used in evaluating the performance of both the Mean Shift Algorithm and Otsu's Method in the Bachelor's Thesis.

All segmentation masks were initially saved using the TIF-format, but were later changed to PNG to allow for previewing on GitHub, using the [following command](https://superuser.com/a/213146):

`find . -name "*.tif" -exec rename -v 's/\.tif$/\.png/i' {} \;`

[Input](./Input) contains the original and binarised expression masks, and the raw ISH-stained images from the Allen Mouse Brain Atlas for both the training and test data sets.

[Output](./Output) contains the produced segmentation masks by both the Mean Shift Algorithm and Otsu's Method for each raw image in Input.