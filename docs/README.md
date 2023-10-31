## Data System - M'23(Project Phase-1)

## Team - 40

## Commands

##### 1- Load matrix

- **Syntax:** `LOAD MATRIX < matrix_name >`
- **Logic:** The original matrix A of dimension n*n is divided into submatrix (block) of size m\*m (not strictly whenever possible). This given us `(n+m-1)/m * (n+m-1)/m` blocks. So we store the matrix in these many number of blocks.
- **Page design:** Let's take a matrix of size 6\*6 and we divide into submatrix of size 4\*4 until possible.
  ![Google Drive Image](https://drive.google.com/uc?id=1SBXM8zXjeRlGgbcOfcleKOYX6Hzr_-EJ)
- If we consider each block to be an element of the original matrix.
- The second and third parameter in naming format tell the position of block in the CSV.
- **number of block access:**
  - Number of read blocks = `0`, because initial read is done from CSV file and not from blocks.
  - Number of write blocks = `(n+m-1)/m * (n+m-1)/m` where n is the size of given matrix and m is `MATRIX_SIZE`, beacause each file is a seperate block and we write in the file.
- **Note:** We are writing 1 block at a time.

##### 2- Print matrix

- **Syntax:** `PRINT MATRIX < matrix_name >`
- **Logic:** First we read `min (matrixSize,20)` rows from the blocks of columns which represent column1 and store into the result vector.
- After getting desired rows, we access blocks one by one from column2 onwards and read only `min (matrixSize,20)` columns and append into corresponding rows.
- And then print the result vector in the terminal.
- **number of block access:**
  - Number of read blocks = It depends on the `MATRIX_SIZE` which represents the submatrix size. If we take small value of MATRIX_SIZE, it will take more read access and vice-versa.
  - Number of write blocks = `0` , because we are just reading the blocks for printing.

##### 3- Transpose matrix

- **Syntax:** `TRANSPOSE MATRIX < matrix_name >`
- **Logic:** The main advantage of block division while loading matrix makes transpose just like a normal matrix.
- For example considering each block as a element of original matrix we tranpose the on diagonal block internally and keep there position same. The remaining strictly upper triangular blocks and strictly lower triangular blocks are transpose internally and then swap with there symmetric pair.
- **Page design:**
  ![Google Drive Image](https://drive.google.com/uc?id=16tzxLYEdEOgJyLAMyI0q9l2nkCrTBeLi)
- **number of block access:**
  - Number of read blocks = total blocks which is equals to `(n+m-1)/m * (n+m-1)/m` where n is the size of given matrix and m is `MATRIX_SIZE`.
  - Number of write blocks = total blocks which is equals to `(n+m-1)/m * (n+m-1)/m` where n is the size of given matrix and m is `MATRIX_SIZE`.
- Each block will have one read and one write.
- **Note:** We are using maximum of 2 blocks at a time.

##### 4- Export matrix

- **Syntax:** `EXPORT MATRIX < matrix_name >`
- **Logic:** We are reading every block row-wise and storing it in a vector.
- After the entire vector is populated we write-back it into the `data` folder.
- **number of block access:**
  - Number of read blocks = total blocks which is equals to `(n+m-1)/m * (n+m-1)/m` where n is the size of given matrix and m is `MATRIX_SIZE`.
  - Number of write blocks = `0`

##### 5- Rename matrix

- **Syntax:** `RENAME MATRIX < matrix_currentname > < matrix_newname >`
- **Logic:** For renaming, we read all the file name from the temp folder and parse each file name by splitting on underscore.
- According to our name format, to rename only first parameter of the block name needs to be change.
- So after replacing the appropriate name in the first parameter, we overwrite the file name.
- **number of block access:**
  - Number of read blocks = `0`
  - Number of write blocks = `0`
  - We are reading only file names and renaming them and during which no block brought into the memory.

##### 6- Check symmetry

- **Syntax:** `CHECKSYMMETRY < matrix_name >`
- **Logic:** For the principle diagonal blocks, we check symmetry internally with the symmetric pair of elements. If any mismatch found, we immediately return false without reading further blocks.
- For non-diagonal blocks, we check symmetry with symmetric pair of blocks at a time. We check the <i,j> element of one block with the <j,i> element of other block in the symmetric pair. On mismatch we immediately return false.
- **number of block access:**
  - Number of write blocks = `0`
  - Number of read blocks = minimum is `1` and maximum is `(n+m-1)/m * (n+m-1)/m` where n is the size of given matrix and m is `MATRIX_SIZE`.
- **Note:** We are using maximum of 2 blocks at a time.

##### 7- Compute

- **Syntax:** `COMPUTE < matrix_name >`
- **Logic:** For the principle diagonal blocks, We subtract each element <i,j> by the element <j,i> in the same block and return the result in a new block.
- For strictly upper triangular blocks, we take the <i,j> element from the block and we take <j,i> element from the corresponding symmetric lower triangular block.
- We do <i,j> - <j,i> operation and store the result in new block at <i,j> location.
- For strictly lower triangular blocks, we take the result of the corresponding upper triangular symmetric blocks, transpose the result, change sign and overwrite the strictly lower triangular block.
- **number of block access:**
  - Number of read blocks = `(n+m-1)/m * (n+m-1)/m` where n is the size of given matrix and m is `MATRIX_SIZE`.
  - We have to read each block once for computation.
  - Number of write blocks = `(n+m-1)/m * (n+m-1)/m` where n is the size of given matrix and m is `MATRIX_SIZE`.
  - Similiarly we will have end up writing once.
- **Note:** We are accessing 2 blocks at a time.

## Assumptions

- We are storing the entire matrix into blocks of matrix of maximum dimension **(MATRIX_SIZE)**.
- To get the number of blocks read/written, MATRIX\*SIZE is used.
- When we load the matrix, the names of divided blocks look like `<mat_name>_Page_R<row>_C<col>`

## Learnings

- Handling (storing, accessing) large data with efficient memory access.
- Handling classes and objects gave us good essence of object-oriented programming.

## Contribution of each member

- **Ujjwal Prakash (2022202009)**
Handled the loading of matrix into various blocks and also combined all the functionalities given in the assignments to run the project as a whole.
`Contribution percentage - 33.33%`
- **Priyank Mahour (2022201047)**
Handled RENAME, EXPORT and PRINT commands.
`Contribution percentage - 33.33%`
- **Prashant Kumar(2022201058)**
Handled TRANSPOSE, CHECKSYMMETRY and COMPUTE commands.
`Contribution percentage - 33.33%`
