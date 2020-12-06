FROM continuumio/miniconda3

SHELL ["/bin/bash", "--login", "-c"]

COPY environment.yml .
COPY startup.sh .

RUN chmod +x startup.sh

RUN conda env create -f environment.yml

RUN conda init bash

RUN echo "conda activate cling" > ~/.bashrc
ENV PATH /opt/conda/envs/env/bin:$PATH

RUN mkdir /notebooks
WORKDIR /notebooks

ENTRYPOINT ["../startup.sh"]