mkdir -p flac;
for f in *.bin; do
  ffmpeg -f s16le -ar 44100 -ac 2 -i "$f" -c:a flac -compression_level 12 -lpc_type cholesky -lpc_passes 2 -min_partition_order 0 -max_partition_order 8 -prediction_order_method search -exact_rice_parameters 1 "flac/${f%.bin}.flac";
done
