while(!shutdown) //将其卸载一个循环中
{
	pthread_mutex_lock(&mutex_);	
	if(<Judgement>)
	{
		pthread_cond_wait(&condition_, &mutex_); //在这里等待，并释放mutex
	}
	if(<Judgement>) //可能出现多个线程竞争，因此<Judgement>条件可能仍然不满足
	{
		pthread_mutex_unlock(&mutex_);
		continue;
	}

	do_something();
	//pthread_cond_signal(&condition_full_); //条件发生变化，可能需要释放某些条件变量。
	pthread_mutex_unlock(&mutex_);
}
