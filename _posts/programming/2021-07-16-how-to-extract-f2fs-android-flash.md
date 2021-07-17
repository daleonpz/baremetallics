---
layout: post
title: How to Extract F2FS Image from Android Device Flash
category: programming
description: How to Extract F2FS Image from Android Device Flash
---

For my work, I had to read the flash memory from an android device, find the filesystem (f2fs), mount it, and extract some files. 
In this post, I share how I did. In case anybody needs this information.

* 
{:toc}

# The Device
The android device was based on a [Rockchip](https://www.rock-chips.com/a/en/) processor, with 7GB of Flash memory, with no access to debugging tools. That means the [Android Debug Bridge](https://www.rock-chips.com/a/en/) was useless in this case.

# The Challenge
As I mentioned, I had to read the flash memory from an android device, find the filesystem (f2fs), mount it, and extract some files.
I used bash commands to solve this challenge. So all the code snippets are from my bash script.

## Reading the Flash Memory
Reading the flash memory was not difficult. I had access to the hardware and with [RockUsb tools](http://opensource.rock-chips.com/wiki_Rockusb) it was straightforward to do it.

```sh
RKDEVELOPTOOL="/home/me/.scripts/rkdeveloptool"

NUM_SECTORS=$(sudo ${RKDEVELOPTOOL} rfi | grep "[0-9]* Sectors" | sed "s,[^0-9.]*,,g")

echo "Number of sectors to read from flash: ${NUM_SECTORS}"

echo "Reading flash memory..."
RAW_FILE="out.bin"
sudo ${RKDEVELOPTOOL} rl 0 "${NUM_SECTORS}" ${RAW_FILE}
```

With `NUM_SECTORS=$(sudo ${RKDEVELOPTOOL} rfi | grep "[0-9]* Sectors" | sed "s,[^0-9.]*,,g")` I retrieve the flash info, and with `grep` and `sed` parse the number of sectors.

## Being Naive
You should install first `f2fs-tools` to work with f2fs images. In ubuntu is like that:

```sh
$ sudo apt-get install -y f2fs-tools 
```

After the installation, I tried to mount directly the device with `mount -o loop`.

```sh
$  sudo mount -o loop -t f2fs out.bin t
mount: wrong fs type, bad option, bad superblock on /dev/loop26, missing codepage or helper program, or other error
```
Of course, it failed. The reason is given by `fsck.f2fs`. 

```sh
$  fsck.f2fs out.bin 
Info: Segments per section = 1
Info: Sections per zone = 1
Info: sector size = 512
Info: total sectors = 15269888 (7456 MB)
        Can't find a valid F2FS superblock at 0x0
        Can't find a valid F2FS superblock at 0x1 
```

What's `fsck.f2fs`?  
"The fsck.f2fs is a tool to check the consistency of an f2fs-formatted partition, which examines whether the filesystem metadata and user-made data are cross-referenced correctly or not." [Ref](https://www.kernel.org/doc/html/latest/filesystems/f2fs.html).

The next step was to find out where is the superblock!.

# Finding The Partition
Each partition had a header, which gives us information about how big the partitions are, how many sectors, how big the block size is, and so on. In the case of f2fs, there is what's called **Superblock**.

"It is located at the beginning of the partition, and there exist two copies to avoid file system crash. It contains basic partition information and some default parameters of f2fs."

I had to find the source code of the f2fs filesystem to know how to recognize the superblock.

The answer was the structure `struct f2fs_super_block`. Lines 34-66 of the [source code](https://www.kernel.org/doc/html/latest/filesystems/f2fs.html).


```c
struct f2fs_super_block {   // According to version 1.1
/* 0x00 */  uint32_t    magic;                  // Magic Number
/* 0x04 */  uint16_t    major_ver;              // Major Version
/* 0x06 */  uint16_t    minor_ver;              // Minor Version
/* 0x08 */  uint32_t    log_sectorsize;         // log2 sector size in bytes
/* 0x0C */  uint32_t    log_sectors_per_block;  // log2 # of sectors per block
/* 0x10 */  uint32_t    log_blocksize;          // log2 block size in bytes
/* 0x14 */  uint32_t    log_blocks_per_seg;     // log2 # of blocks per segment
/* 0x18 */  uint32_t    segs_per_sec;           // # of segments per section
/* 0x1C */  uint32_t    secs_per_zone;          // # of sections per zone
/* 0x20 */  uint32_t    checksum_offset;        // checksum offset inside super block
/* 0x24 */  uint64_t    block_count;            // total # of user blocks
/* 0x2C */  uint32_t    section_count;          // total # of sections
/* 0x30 */  uint32_t    segment_count;          // total # of segments
/* 0x34 */  uint32_t    segment_count_ckpt;     // # of segments for checkpoint
/* 0x38 */  uint32_t    segment_count_sit;      // # of segments for SIT
/* 0x3C */  uint32_t    segment_count_nat;      // # of segments for NAT
/* 0x40 */  uint32_t    segment_count_ssa;      // # of segments for SSA
/* 0x44 */  uint32_t    segment_count_main;     // # of segments for main area
/* 0x48 */  uint32_t    segment0_blkaddr;       // start block address of segment 0
/* 0x4C */  uint32_t    cp_blkaddr;             // start block address of checkpoint
/* 0x50 */  uint32_t    sit_blkaddr;            // start block address of SIT
/* 0x54 */  uint32_t    nat_blkaddr;            // start block address of NAT
/* 0x58 */  uint32_t    ssa_blkaddr;            // start block address of SSA
/* 0x5C */  uint32_t    main_blkaddr;           // start block address of main area
/* 0x60 */  uint32_t    root_ino;               // root inode number
/* 0x64 */  uint32_t    node_ino;               // node inode number
/* 0x68 */  uint32_t    meta_ino;               // meta inode number
/* 0x6C */  uint8_t     uuid[F2FS_UUID_SIZE];   // 128-bit uuid for volume
/* 0x7C */  uint16_t    volume_name[F2FS_LABEL_SIZE];   // volume name
/* 0x47C */ uint32_t    extension_count;        // # of extensions below
/* 0x480 */ uint8_t     extension_list[64][8];  // extension array
} PACKED;
```

The start of the superblock structure is the **Magic Number**. Cool name btw :).  Anyway, the magic number is defined at line 25.

```c
#define F2FS_MAGIC      0xF2F52010  // F2FS Magic Number
```

This number should be somewhere in the flash memory. If we find its address, then we can retrieve the info from the superblock.

For this job I used `xxd`. I think with `hexdump` could have also worked.

```sh
xxd -s "${_offset}" -l 16 ${RAW_FILE}  | grep "400: 1020 f5f2" | sed 's/:.*//' 
```

Things to notice:
- I search with `grep` for **1020 f5f2** and not **f2f5 2010**. That's because I created a test f2fs partition to understand better the superblock structure and I noticed that bytes were inverted. 

- In the beginning, there is a **400**. The superblock has an offset of `0x400`, defined at line 29 of the [source code](https://www.kernel.org/doc/html/latest/filesystems/f2fs.html).  

```c
#define F2FS_SB1_OFFSET     0x400       // offset for 1:st superblock
```

I automated the superblock search with:

```sh
echo "Retrieving f2fs volume..."

ADDRESS=()
_STEP=0x1000 # 4MB, because for us is important to find an address of the form 0xYYYYX400

FILESIZE=$(stat --printf="%s" ${RAW_FILE})
for _offset in $(seq $((0x0400)) ${_STEP} "$((FILESIZE/2))")
do    
## 16 bytes are more than enough for our purposes
    ADDRESS+=( $(xxd -s "${_offset}" -l 16 ${RAW_FILE}  | grep "400: 1020 f5f2" | sed 's/:.*//' ) )
done

if [ -z "${ADDRESS[*]}" ]; then
    echo "Didn't find any f2fs volumen."
    echo "Exit"
    exit 1
fi

ADDRESS_INT=$(( "0x${ADDRESS[0]}" ))

echo "Done."

```

# Extract the F2FS Image
Now that we have the address, the next problem is to find out how big the image is. With this information, we can extract the f2fs partition as a whole. 

There are two ways to find it out. 

## Non-scriptable Way To Get Image size
First, copy a big chunk (16 MB) from the image to another file `__Image`.  

```sh
$  dd if=out.bin iflag=skip_bytes,count_bytes skip=$((0xaa800000)) count=$((0x1000000)) bs=4096 of=__Image      
4096+0 records in                                                       
4096+0 records out                                                      
16777216 bytes (17 MB, 16 MiB) copied, 0,0258219 s, 650 MB/s            
```

and then I used `fsck.f2fs` to get info about the extracted file.

```sh
$  fsck.f2fs __Image                            
Info: Segments per section = 1                                          
Info: Sections per zone = 1                                             
Info: sector size = 512                                                 
Info: total sectors = 32768 (16 MB)                                     
Info: MKFS version                                                      
  "Linux version 4.4.154 (wenzel@werkstatt) (gcc version 6.3.1 20170404 (Linaro GCC 6.3-2017.05) ) #7 SMP PREEMPT Thu Apr 16 12:07:11 CE ST 2020"
Info: FSCK version from "Linux version 4.4.154 (wenzel@werkstatt) (gcc version 6.3.1 20170404 (Linaro GCC 6.3-2017.05) ) #1 SMP PREEMPT Fri Mar 5 12:45:4 8 CET 2021"
    to "Linux version 5.4.0-77-generic (buildd@lgw01-amd64-021) (gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)) #86~18.04.1-Ubuntu SMP
 Fri Jun 18 01:23:22 UTC 2021"
Info: superblock features = 0 : 
Info: superblock encrypt level = 0, salt = 00000000000000000000000000000000
Info: total FS sectors = 9674752 (4724 MB)
Info: CKPT version = 20e
Info: checkpoint state = 44 :  crc compacted_summary sudden-power-off
[ASSERT] (sanity_check_nid: 417)  --> nid[0x3] nat_entry->ino[0x3] footer.ino[0x0]
...
...
```

`fsck.f2fs` gives more info, but for us the key point is this line:

```sh
Info: total FS sectors = 9674752 (4724 MB)
```

With that info we can run again `dd`.

```sh
$  dd if=out.bin iflag=skip_bytes,count_bytes skip=$((0xaa800000)) count=$((4724*1024*1024)) bs=4096 of=__Image.img
```

Let's test again `__Image.img`.

```sh
$  fsck.f2fs __Image.img 
Info: Segments per section = 1
Info: Sections per zone = 1
Info: sector size = 512
Info: total sectors = 9674752 (4724 MB)
Info: MKFS version
  "Linux version 4.4.154 (wenzel@werkstatt) (gcc version 6.3.1 20170404 (Linaro GCC 6.3-2017.05) ) #7 SMP PREEMPT Thu Apr 16 12:07:11 CEST 2020"
Info: FSCK version
  from "Linux version 4.4.154 (wenzel@werkstatt) (gcc version 6.3.1 20170404 (Linaro GCC 6.3-2017.05) ) #1 SMP PREEMPT Fri Mar 5 12:45:48 CET 2021"
    to "Linux version 5.4.0-77-generic (buildd@lgw01-amd64-021) (gcc version 7.5.0 (Ubuntu 7.5.0-3ubuntu1~18.04)) #86~18.04.1-Ubuntu SMP Fri Jun 18 01:23:22 UTC 2021"
Info: superblock features = 0 : 
Info: superblock encrypt level = 0, salt = 00000000000000000000000000000000
Info: total FS sectors = 9674752 (4724 MB)
Info: CKPT version = 20e
Info: checkpoint state = 44 :  crc compacted_summary sudden-power-off

[FSCK] Unreachable nat entries                        [Ok..] [0x0]
[FSCK] SIT valid block bitmap checking                [Ok..]
[FSCK] Hard link checking for regular file            [Ok..] [0x0]
[FSCK] valid_block_count matching with CP             [Ok..] [0x8fd6]
[FSCK] valid_node_count matcing with CP (de lookup)   [Ok..] [0x283]
[FSCK] valid_node_count matcing with CP (nat lookup)  [Ok..] [0x283]
[FSCK] valid_inode_count matched with CP              [Ok..] [0x261]
[FSCK] free segment_count matched with CP             [Ok..] [0x8d2]
[FSCK] next block offset is free                      [Ok..]
[FSCK] fixing SIT types
[FSCK] other corrupted bugs                           [Ok..]

Done.
```

Everything looks good.


**Notes about dd**
In case you were wondering about the functionality of `count_bytes` and `skip_bytes`.

- `count_bytes`
      Interpret the `count=` operand as a byte count, rather than a
      block count, which allows specifying a length that is not a
      multiple of the I/O block size.  This flag can be used only
      with `iflag`.

- `skip_bytes`
      Interpret the `skip=` operand as a byte count, rather than a
      block count, which allows specifying an offset that is not a
      multiple of the I/O block size.  This flag can be used only
      with `iflag`.


## Scriptable Way To Get Image size
The way I liked more was by reading the values of the `f2fs_super_block` structure. Which values?
- Sector size: `log_sectorsize` with offset `0x08` and  4 bytes long.
- Number of sectors per block: `log_sectors_per_block` with offset `0x0C` and 4 bytes long.
- Number of blocks: `block_count` with offset `0x24` and 8 bytes long.

With `xxd`, the right offset and number of bytes, I read the information above. Note that I also had to flip the bytes like the case of the magic number.

Note that **Sector size** and **Number of sectors per block** are in log2. That means that if:

```c
uint32_t log_sectorsize = 0x00000003;
uint32_t sectorsize = pow(2, log_sectorsize);
```

The `ADDRESS_INT` variable in my script comes from section before. Which is the **magic number** address.

```sh
START_ADDRESS=$(( ADDRESS_INT - 0x400 )) # f2fs struct has an offset of 0x400
printf 'Info: image start address = 0x%X\n' ${START_ADDRESS} 

LOG2_SECTOR_SIZE=$(xxd -s $((ADDRESS_INT + 0x08)) -l 4 -p ${RAW_FILE} | fold -w2 | tac | tr -d "\n")
LOG2_SECTORS_PER_BLOCK=$(xxd -s $((ADDRESS_INT + 0x0C)) -l 4 -p ${RAW_FILE} | fold -w2 | tac | tr -d "\n")
BLOCK_COUNT=$(xxd -s $((ADDRESS_INT + 0x24)) -l 8 -p ${RAW_FILE} | fold -w2 | tac | tr -d "\n")

SECTOR_SIZE=$(( 2**"0x${LOG2_SECTOR_SIZE}" ))
SECTORS_PER_BLOCK=$(( 2**"0x${LOG2_SECTORS_PER_BLOCK}" ))
BLOCK_COUNT=$(( "0x${BLOCK_COUNT}" ))

echo "Info: sector size = ${SECTOR_SIZE}"
echo "Info: sectors per block = ${SECTORS_PER_BLOCK}"
echo "Info: block count = ${BLOCK_COUNT}"

NUM_BYTES=$(( BLOCK_COUNT * SECTORS_PER_BLOCK * SECTOR_SIZE  ))
echo "Info: total of bytes to read = ${NUM_BYTES} ($((NUM_BYTES/1024/1024)) MB)"

F2FS_IMAGE="f2fs_image.img"
echo "Extracting f2fs volumen..."
dd if=${RAW_FILE} iflag=skip_bytes,count_bytes skip=${START_ADDRESS}  count=${NUM_BYTES} bs=4096 of=${F2FS_IMAGE}
echo "Done."
```

The results are the same. You can prove it with `diff` or by running again `fsck.f2fs`.

# Mounting the image
Finally, the last part is just mounting the image and have access to files.

```sh
MOUNTPOINT="my_mounting_point"
mkdir -p ${MOUNTPOINT}
mount -o loop -t f2fs ${F2FS_IMAGE} ${MOUNTPOINT}
```

# The Whole Script

```sh
#!/bin/bash
RAW_FILE="out.bin"

ADDRESS=()
FILESIZE=$(stat --printf="%s" ${RAW_FILE})

for _offset in $(seq $((0x0400)) $(((0x1000))) "$((FILESIZE/2))")
do    
    ADDRESS+=( $(xxd -s "${_offset}" -l 16 ${RAW_FILE}  | grep "400: 1020 f5f2" | sed 's/:.*//' ) )
done

if [ -z "${ADDRESS[*]}" ]; then
    echo "Didn't find any f2fs volumen."
    echo "Exit"
    exit 1
fi

echo "Done."

ADDRESS_NUM=$(( "0x${ADDRESS[0]}" ))
START_ADDRESS=$(( ADDRESS_NUM - 0x400 ))
printf 'Info: image start address = 0x%X\n' ${START_ADDRESS} 

LOG2_SECTOR_SIZE=$(xxd -s $((ADDRESS_NUM + 0x08)) -l 4 -p ${RAW_FILE} | fold -w2 | tac | tr -d "\n")
LOG2_SECTORS_PER_BLOCK=$(xxd -s $((ADDRESS_NUM + 0x0C)) -l 4 -p ${RAW_FILE} | fold -w2 | tac | tr -d "\n")
BLOCK_COUNT=$(xxd -s $((ADDRESS_NUM + 0x24)) -l 8 -p ${RAW_FILE} | fold -w2 | tac | tr -d "\n")

SECTOR_SIZE=$(( 2**"0x${LOG2_SECTOR_SIZE}" ))
SECTORS_PER_BLOCK=$(( 2**"0x${LOG2_SECTORS_PER_BLOCK}" ))
BLOCK_COUNT=$(( "0x${BLOCK_COUNT}" ))

echo "Info: sector size = ${SECTOR_SIZE}"
echo "Info: sectors per block = ${SECTORS_PER_BLOCK}"
echo "Info: block count = ${BLOCK_COUNT}"

NUM_BYTES=$(( BLOCK_COUNT * SECTORS_PER_BLOCK * SECTOR_SIZE  ))
echo "Info: total of bytes to read = ${NUM_BYTES} ($((NUM_BYTES/1024/1024)) MB)"

F2FS_IMAGE="f2fs_image.img"
echo "Extracting f2fs volumen..."
dd if=${RAW_FILE} iflag=skip_bytes,count_bytes skip=${START_ADDRESS}  count=${NUM_BYTES} bs=4096 of=${F2FS_IMAGE}
echo "Done."

MOUNTPOINT="___mount"

echo "Mounting image..."
mkdir -p ${MOUNTPOINT} 

if ! mount -o loop -t f2fs ${F2FS_IMAGE} ${MOUNTPOINT}; then
    echo "Couldn't mount image."
    echo "Exit."
    exit 1
fi
echo "Done."

```

# Conclusion
For my work, I had to read the flash memory from an android device, find the filesystem (f2fs), mount it, and extract some files.
In this post, I showed a way to do it. The same logic can be applied to any filesystem. 

I hope you enjoyed the post.
