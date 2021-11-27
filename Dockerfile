FROM ternaryclub/taylor
# Add main executable
ADD terryc /terryc
# Add testing file and run
ADD test.try /test.try
CMD ["/terryc", "./test.try"]