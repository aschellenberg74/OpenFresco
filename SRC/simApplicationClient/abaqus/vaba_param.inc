      implicit real (a-h,o-z)
      parameter (j_sys_Dimension = 1)
      parameter( n_vec_Length = 544  )
      parameter( maxblk = n_vec_Length  )

      parameter(i_ipm_sta = -6)
      character*5 j_ipm_Error
      parameter(j_ipm_Error = "Error")
      parameter(j_ipm_Aborted = 20)
      parameter(r_MaxVal = 1.d+30)
