#define CL_TARGET_OPENCL_VERSION 220

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <CL/cl.h>
 
#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)
#define MAX_ARRAY_SIZE (100)

const u_long size = 100;
//float matrix[size][size];
float matrixOne[MAX_ARRAY_SIZE * MAX_ARRAY_SIZE];
float vector[MAX_ARRAY_SIZE];
float outputVector[MAX_ARRAY_SIZE];

int main()
{

    
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_mem memobj = NULL;
    cl_mem memobjMatrix = NULL;
    cl_mem memobjVector = NULL;
    cl_mem memobjOutputVector = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_platform_id platform_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    cl_int ret1;
    cl_int ret2;
    cl_int ret3;

    
    char string[MEM_SIZE];
    
    FILE *fp;
    char fileName[] = "./matrix.cl";
    char *source_str;
    size_t source_size;
    
    // Load the source code containing the kernel
    fp = fopen(fileName, "r");
    if (!fp)
    {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);
    
    perror("Okay source read");

    /* Get Platform and Device Info */
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    if(ret != CL_SUCCESS)
    {
        printf("%s %d\n","error1",ret);
        exit(1);
    }

    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    if(ret != CL_SUCCESS)
    {
        printf("%s %d","error2",ret);
        exit(1);
    }
    
    // Create OpenCL context
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    
    // Create Command Queue 
    command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);




    for (int i = 0; i < size; i++)
    {
        matrixOne[i* size + i]=1;
        vector[i]=i;
    }


    // Create Memory Buffer
    memobjMatrix = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * size * sizeof(float), matrixOne, &ret1);
    if(ret1 != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant create matrix buffer",ret1);
        exit(1);
    }

    perror("Okey create buffer matrix");
    memobjVector = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size * sizeof(float), vector, &ret2);
    if(ret2 != CL_SUCCESS)
    {
        printf("%s","Cant create vector buffer");
        exit(1);
    }
    memobjOutputVector = clCreateBuffer(context, CL_MEM_READ_ONLY, size * sizeof(float), NULL, &ret3);
    if(ret3 != CL_SUCCESS)
    {
        printf("%s","Cant create outputVector buffer");
        exit(1);
    }



    // Create Kernel Program from the source
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
    (const size_t *)&source_size, &ret);
    if(ret != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant create programm source",ret);
        exit(1);
    }

    perror("Okey create programm with sourse");
    
    //Build Kernel Program
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (ret == CL_BUILD_PROGRAM_FAILURE)
    {
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        // Allocate memory for the log
        char *log = (char *) malloc(log_size);

        // Get the log
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        

        // Print the log
        printf("%s\n", log);
    }


    // if(ret!=CL_SUCCESS){
    //     printf("%s %d","error3",ret);
    //     exit(0);

    // }
    
    /* Create OpenCL Kernel */
    kernel = clCreateKernel(program, "getMatrixSum", &ret);
    if(ret != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant create programm kernel",ret);
        exit(1);
    }

    perror("Okey create opencl kernel");

    
    /* Set OpenCL Kernel Parameters */
    ret1 = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjMatrix);
    if(ret1 != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant create set kernel parameter 0",ret1);
        exit(1);

    }
    ret2 = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobjVector);
    if(ret2 != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant create set kernel parameter 1",ret2);
        exit(1);

    }
    ret3 = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memobjOutputVector);
    if(ret3!=CL_SUCCESS){
        printf("%s  %d\n","Cant create set kernel parameter 2",ret3);
        exit(1);

    }
    ret3 = clSetKernelArg(kernel, 3, sizeof(u_long),(void*)&size);
    if(ret3 != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant create set kernel arg",ret3);
        exit(1);

    }

    perror("Okey set kernel parameters");//---------------------------------------




    /* Execute OpenCL Kernel */
    //sxala
    // ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
    // if(ret!=CL_SUCCESS){
    //     printf("%s  %d\n","Cant enque task",ret);
    //     exit(1);
    // }

    /* Execute OpenCL Kernel */
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, 0, &size, 0, 0, 0, 0);
    if(ret != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant enque task",ret);
        exit(1);
    }

    perror("Okey execute opencl kernel");

    /* Copy results from the memory buffer */
    ret = clEnqueueReadBuffer(command_queue, memobjOutputVector, CL_TRUE, 0,
    sizeof(float) *size,outputVector, 0, NULL, NULL);
    if(ret != CL_SUCCESS)
    {
        printf("%s  %d\n","Cant enque read buffer",ret);
        exit(1);
    }

    sleep(3);

    perror("Okey reading results from memory");

    // Display Result
    //puts();
    for (int i = 0; i < size; i++)
    {
        printf("%f ",outputVector[i]);
    }

    perror("print result---------------------");
    
    /* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memobj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    
    free(source_str);
    
    return 0;
}