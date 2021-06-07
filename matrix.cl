


__kernel void getMatrixSum(__global const float *matrix,
                            __global const float *vector,
                            __global float* outputVector,
                                            ulong n){

        
        ulong num=(ulong)get_global_id(0);
        float sum=0;
        if(num<n){
            for(ulong i=0;i<n;i++)
            {
                sum+=matrix[num*n+i]*vector[i];
                
            }
            outputVector[num]=sum;
        }


}