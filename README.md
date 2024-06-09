# NeymarkTestApp
Some test application for students

## Тестирование

### Добавляем обработку сигналов (signal handler)
``` bash
$ ./simple-service -sr
$ kill -s SIGINT `cat /tmp/server.pid`
```

### Добавляем логирование (syslog)
``` bash
$ ./simple-service -sr
$ ./simple-service -crm0
$ journalctl --user | grep simple-service
```

### Разрабатываем библиотеку для создания сервиса (shared library)
``` bash
$ cmake CMakeLists.txt
$ make
$ readelf -d simple-service-1.0.0 | grep NEEDED
```

