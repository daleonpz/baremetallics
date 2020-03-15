const char main[] ={
  0x55,                   	
  0x48, 0x89, 0xe5,
  0xb8 , 0x01 , 0x00 , 0x00 , 0x00,
  0xbb , 0x01 , 0x00 , 0x00 , 0x00,
  0xbe , 0xc7 , 0x04 , 0x40 , 0x00,    	
  0xba , 0x0a , 0x00 , 0x00 , 0x00,	
  0x0f , 0x05 ,
  0xb8 , 0x3c , 0x00 , 0x00 , 0x00,
  0x0f , 0x05,
  // <message>
  0x68, 0x6f , 0x6c , 0x69 , 0x20 ,
  0x62,
  0x6f,               
  0x6c,                
  0x69 , 0x0a , 0xb8 , 0x00 , 0x00 , 0x00,
  0x00 , 0x5d , 0xc3 ,
  0x0f , 0x1f , 0x84 , 0x00 , 0x00 , 0x00, 0x00, 	
  0x00 
};

/*
 * objdump output from 5.c
 00000000004004a6 <main>:
  4004a6:	55                   	push   %rbp
  4004a7:	48 89 e5             	mov    %rsp,%rbp
  4004aa:	b8 01 00 00 00       	mov    $0x1,%eax
  4004af:	bb 01 00 00 00       	mov    $0x1,%ebx
  4004b4:	be c7 04 40 00       	mov    $0x4004c7,%esi
  4004b9:	ba 0a 00 00 00       	mov    $0xa,%edx
  4004be:	0f 05                	syscall 
  4004c0:	b8 3c 00 00 00       	mov    $0x3c,%eax
  4004c5:	0f 05*
 * 
 * the address 0x4004c7 is where the message start..
 * ascii hex code for message
 *
 *	68 6f 6c 69 20 62 6f 6c 69 0a b8 00 00 00  
 *	h  o  l  i     b  o  l  i  \n 
 *
 * and of course the message is not in memory T.T
 * so let's save some mem :)
 *
 *  <message>
 *  4004c7:	68 6f 6c 69 20       	pushq  $0x20696c6f
  4004cc:	62                   	(bad)  
  4004cd:	6f                   	outsl  %ds:(%rsi),(%dx)
  4004ce:	6c                   	insb   (%dx),%es:(%rdi)
  4004cf:	69 0a b8 00 00 00    	imul   $0xb8,(%rdx),%ecx
  4004d5:	00 5d c3             	add    %bl,-0x3d(%rbp)
  4004d8:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
  4004df:	00   
 *
 * 
 * I need to change the message address to 0x601061*
  601061:	68 6f 6c 69 20       	pushq  $0x20696c6f

is not working.... uhmmm.. maybe it's because is in .data section, let's put in the ".text" section by make it const.
By doing so, now main is in .rodata. Of course, we should update the message address

new address:
  400581:	68 6f 6c 69 20       	pushq  $0x20696c6f

ohhh yeahhh!!! it finally works!! wu-ju! DONE

 * */

