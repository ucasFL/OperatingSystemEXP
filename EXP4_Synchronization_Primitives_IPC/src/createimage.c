#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


Elf32_Phdr * read_exec_file(FILE **execfile, Elf32_Ehdr **ehdr)
{
	//*execfile = fopen(filename, "rb");
	//assert(execfile);
	*ehdr = (Elf32_Ehdr *) malloc (512 * sizeof(uint8_t));
	fread( *ehdr, 512, 1, *execfile );
	//printf( "%d\n", (*ehdr)->e_phoff );
	Elf32_Phdr *phdr;
	phdr = (Elf32_Phdr *) malloc ((512 - (*ehdr)->e_phoff) * sizeof(uint8_t));
	fseek(*execfile, (*ehdr)->e_phoff, SEEK_SET);
	fread (phdr, 512 - (*ehdr)->e_phoff, 1, *execfile );
	//printf( "%d\n", phdr->p_memsz );
	return phdr;
}

void write_boot(FILE **imagefile, FILE *boot_file, Elf32_Ehdr *boot_header,
		Elf32_Phdr *boot_phdr, int *count)
{
	int i;
	for ( i = 0; i < boot_header->e_phnum; i++ ) {
		if( boot_phdr[i].p_type == PT_LOAD ){//need write to image
			*count += boot_phdr[i].p_filesz;
			int test = fseek(boot_file, boot_phdr[i].p_offset, SEEK_SET);//seek to the beginning of the section
			assert(!test);
			//read to buf and write to image
			uint8_t buf[boot_phdr[i].p_filesz];
			fread(buf, boot_phdr[i].p_filesz, 1, boot_file);
			fwrite(buf, boot_phdr[i].p_filesz, 1, *imagefile);
		}
	}
	//printf( "countboot = %d\n", *count );
	//write zero to the left bytes
	uint8_t zero[512 - *count - 2 ];
	for ( i = 0; i < 512 - *count -2 ; i++ ) {
		zero[i] = 0;
	}
	//write 0x55 and 0xaa to the last two bytes of the first sector.
	uint8_t end[2] = {85,170};
	fwrite(zero, 512 - *count - 2, 1, *imagefile);
	fwrite(end, 2, 1, *imagefile);
}

void write_kernel(FILE **image_file, FILE *kernel_file, Elf32_Ehdr *kernel_ehdr, Elf32_Phdr *kernel_phdr, int *count)
{
	int i;
	for ( i = 0; i < kernel_ehdr->e_phnum; i++ ) {
		if( kernel_phdr[i].p_type == PT_LOAD ){//need write to image
			*count += kernel_phdr[i].p_filesz;
			int test = fseek(kernel_file, kernel_phdr[i].p_offset, SEEK_SET);//seek to the beginning of the section
			assert(!test);
			//read to buf and write to image
			uint8_t buf[kernel_phdr[i].p_filesz];
			fread(buf, kernel_phdr[i].p_filesz, 1, kernel_file);
			fwrite(buf, kernel_phdr[i].p_filesz, 1, *image_file);
		}
	}
	//printf( "countkernel = %d\n", *count );
	//write zero to the left bytes
	int co = *count % 512;
	uint8_t zero[512 - co];
	for ( i = 0; i < 512 - co; i++ ) {
		zero[i] = 0;
	}
	fwrite(zero, 512 - co, 1, *image_file);
}

int count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr){
	int i, count = 0;
	for ( i = 0; i < kernel_header->e_phnum; i++ ) {
		if ( kernel_phdr[i].p_type == PT_LOAD )
		  count += kernel_phdr[i].p_filesz;
	}
	return (count + 511) >> 9;
}

void record_kernel_sectors(FILE **image_file, int num_sec){
	int test = fseek(*image_file, 0x1f8, SEEK_SET);//seek to the second inst of bootblock
	assert( !test );
	int bytes = num_sec * 512; //LI $6, 0x200 * num_sec
	//printf ("!!!!!!!!!!!!!\n%d\n %d\n !!!!!!!!!!!!",num_sec, bytes);
	fwrite(&bytes, 4, 1, *image_file );//modify the image file
}

void extended_opt(Elf32_Ehdr *boot_ehdr, Elf32_Ehdr *kernel_ehdr, 
			Elf32_Phdr *boot_phdr,  Elf32_Phdr *kernel_phdr, int cb, int ck, int cp1, int cp2, int cp3, int cps){
	struct stat buf_boot, buf_kernel;
	stat("bootblock", &buf_boot);
	stat("kernel", &buf_kernel);
	printf("\nlength_of_bootblock = %d\n", (int)buf_boot.st_size);
	printf( "p_offset = %d, p_filesz = %d\n",boot_phdr->p_offset, boot_phdr->p_filesz );
	printf( "length_of_kernel = %d\n", (int)buf_kernel.st_size );
	
	int ker_sec = count_kernel_sectors(kernel_ehdr, kernel_phdr);
	printf( "kernel_sectors: = %d\n", ker_sec );
	printf( "p_offset = %d, p_filesz = %d\n",kernel_phdr->p_offset, kernel_phdr->p_filesz );
	printf( "kernel_phdr->p_offset = %d\n\n", kernel_phdr->p_offset );

	printf( "bootblock image info\n" );
	int boot_sec = count_kernel_sectors(boot_ehdr, boot_phdr);
	printf( "sectors = %d\n", boot_sec );
	printf( "offset of segment in the file: 0x%x\n", boot_phdr->p_offset );
	printf("the image's virtural address of segment in memory: 0x%x\n", boot_phdr->p_vaddr);
	printf("the file image size of segment: 0x%x\n", boot_phdr->p_filesz );
	printf("the memory image size of segment: 0x%x\n", boot_phdr->p_memsz);
	printf("size of write to the OS image: 0x%x\n", cb);
	printf("padding up to 0x200\n\n" );

	printf( "kernel image info\n" );
	printf( "sectors = %d\n", ker_sec );
	printf( "offset of segment in the file: 0x%x\n", kernel_phdr->p_offset );
	printf("the image's virtural address of segment in memory: 0x%x\n", kernel_phdr->p_vaddr);
	printf("the file image size of segment: 0x%x\n", kernel_phdr->p_filesz);
	printf("the memory image size of segment: 0x%x\n", kernel_phdr->p_memsz);
	printf("size of write to the OS image: 0x%x\n", ck);
	printf("padding up to 0x200\n\n" );

	printf( "process 1 sectors = %d\n", cp1 );
	printf( "process 2 sectors = %d\n", cp2 );
	printf( "process 3 sectors = %d\n", cp3 );
	printf( "process 4 sectors = %d\n", cps );
	
}

void write_zero( FILE **fp, int sec_num ){
	int bytes_num = 512 * sec_num;
	int zero[bytes_num];
	for ( int i = 0; i < bytes_num; i++ )
	  zero[i] = 0;
	fwrite( zero, bytes_num, 1, *fp );
}


int main(int argc, char *argv[]){
	if (argc < 3)
	  exit(0);
	Elf32_Ehdr *ehdr, *ehdrk, *ep1, *ep2, *ep3, *eps;
	Elf32_Phdr *phdr, *phdrk, *pp1, *pp2, *pp3, *pps;
	FILE *fp, *fpk, *p1, *p2, *p3, *ps;
	FILE *imfp = fopen("image", "w+");
	p1 = fopen(argv[4], "rb");
	p2 = fopen(argv[5], "rb");
	p3 = fopen(argv[6], "rb");
	ps = fopen(argv[7], "rb");

	if(strcmp(argv[2], "bootblock") || strcmp(argv[3], "kernel"))
	  exit(0);

	fp = fopen(argv[2], "rb");
	assert(fp);
	fpk = fopen(argv[3], "rb");
	assert(fpk);

    phdr = read_exec_file(&fp,  &ehdr);
	phdrk = read_exec_file(&fpk,  &ehdrk);
	pp1 = read_exec_file(&p1, &ep1);
	pp2 = read_exec_file(&p2, &ep2);
	pp3 = read_exec_file(&p3, &ep3);
	pps = read_exec_file(&ps, &eps);

	int ker_sec, p1_sec, p2_sec, boot_sec, p3_sec, ps_sec;
	ker_sec = count_kernel_sectors(ehdrk, phdrk);
	p1_sec = count_kernel_sectors(ep1, pp1);
	p2_sec = count_kernel_sectors(ep2, pp2);
	boot_sec = count_kernel_sectors(ehdr, phdr);
	p3_sec = count_kernel_sectors(ep3, pp3);
	ps_sec = count_kernel_sectors(eps, pps);

	int cb = 0, ck = 0;
	write_boot(&imfp, fp, ehdr, phdr, &cb);//write bootblock to image
	write_kernel(&imfp, fpk, ehdrk, phdrk, &ck);//write kernel to image
	write_zero( &imfp, 255 - ker_sec );

	int temp1, temp2, temp3, temps;
	temp1 = temp2 = temp3 = temps = 0;
	write_kernel(&imfp, p1, ep1, pp1, &temp1 );
	write_zero( &imfp, 16 - p1_sec );

	write_kernel(&imfp, p2, ep2, pp2, &temp2);
	write_zero(&imfp, 16 - p2_sec);

	write_kernel( &imfp, p3, ep3, pp3, &temp3 );
	write_zero( &imfp, 16 - p3_sec);

	write_kernel( &imfp, ps, eps, pps, &temps );

	record_kernel_sectors( &imfp, 303 + p3_sec );
	//printf ("===============\n %d\n==============", 303+p3_sec);

	if(!strcmp(argv[1], "--extended"))
	  extended_opt(ehdr, ehdrk, phdr, phdrk, cb, ck, p1_sec, p2_sec, p3_sec, ps_sec);

	fclose(fp);
	fclose(fpk);
	fclose(imfp);
	fclose(p1);
	fclose(p2);
}
