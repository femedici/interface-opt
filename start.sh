#!/bin/bash
source .env

echo "Iniciando Backend na porta $PORT..."
cd back && npm start &

sleep 3  # Espera um pouco para garantir que o backend iniciou

echo "Iniciando Frontend..."
cd ../$FRONTEND_PATH && $FRONTEND_COMMAND
