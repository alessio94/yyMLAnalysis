FROM gitlab-registry.cern.ch/atlas/athena/analysisbase:25.2.27
ADD . /yyML/yyMLAnalysisCode
WORKDIR /yyML/build
RUN source ~/release_setup.sh &&  \
    sudo chown -R atlas /yyML && \
    cmake ../yyMLAnalysisCode && \
    make -j4

