for %%i in (*.hlsl) do (
  fxc /T vs_5_0 /E VxShader /Fo "%%~ni.vsh" "%%i"
  fxc /T ps_5_0 /E PxShader /Fo "%%~ni.psh" "%%i"
)

pause