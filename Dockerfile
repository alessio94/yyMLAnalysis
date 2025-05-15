FROM gitlab-registry.cern.ch/atlas/athena/analysisbase:25.2.27
ADD . /yyML/yyMLAnalysisCode
WORKDIR /hww/build
RUN source ~/release_setup.sh &&  \
    sudo chown -R atlas /hww && \
    cmake ../yyMLAnalysisCode && \
    make -j4

