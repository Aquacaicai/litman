<script setup>
import { MagnifyingGlassIcon, ChartBarIcon, UsersIcon, PlusIcon, Bars3Icon } from '@heroicons/vue/24/outline'
</script>

<template>
  <div class="drawer lg:drawer-open">
    <input id="my-drawer" type="checkbox" class="drawer-toggle" />
    <div class="drawer-content flex flex-col">
      <div class="navbar bg-base-100 lg:hidden">
        <div class="flex-none">
          <label for="my-drawer" class="btn btn-square btn-ghost drawer-button">
            <Bars3Icon class="size-6" />
          </label>
        </div>
        <div class="flex-1">
          <a class="btn btn-ghost normal-case text-xl">Litman</a>
        </div>
      </div>

      <div class="p-4 md:p-6">
        <router-view></router-view>
      </div>
    </div>
    <div class="drawer-side">
      <label for="my-drawer" class="drawer-overlay"></label>
      <aside class="bg-base-200 w-64 h-full">
        <div class="p-4">
          <h1 class="text-2xl font-bold text-primary mb-6 w-center text-center">Litman</h1>
          <ul class="menu menu-md bg-base-200 rounded-box space-y-1">
            <li>
              <router-link to="/" active-class="active">
                <MagnifyingGlassIcon class="size-5" />
                Literature Search
              </router-link>
            </li>
            <li>
              <router-link to="/statistics" active-class="active">
                <ChartBarIcon class="size-5" />
                Statistics
              </router-link>
            </li>
            <li>
              <router-link to="/collaborations" active-class="active">
                <UsersIcon class="size-5" />
                Collaborations
              </router-link>
            </li>
            <li>
              <router-link to="/manage" active-class="active">
                <PlusIcon class="size-5" />
                Manage Literature
              </router-link>
            </li>
          </ul>
        </div>
      </aside>
    </div>
  </div>
</template>


<style></style>
