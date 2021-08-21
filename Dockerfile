FROM ternaryclub/taylor
# Add main executable
ADD terry /terry
# Add testing file and run
ADD test.try /test.try
CMD ["/terry", "./test.try"]